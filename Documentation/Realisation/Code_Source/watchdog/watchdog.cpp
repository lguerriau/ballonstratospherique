/**
@file watchdog.cpp
@brief implemantation de la classe Watchdog
@version 1.0
@author Harold KALO
@date 26/05/2026
@details Classe modélisant la surveillance de fichiers .bmp du poste Windows
*/

#include "watchdog.h"
#include "ui_watchdog.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <cassert>
#include <QCoreApplication>
#include <unistd.h>

/**
 * @brief Watchdog::Watchdog
 * @param parent pointeur vers le widget parent, par défaut nullptr
 * @author Harold KALO
 */
Watchdog::Watchdog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    this->ui->setupUi(this);

    if (this->ui == nullptr) {
        qDebug() << "[ERREUR CRITIQUE] : Le widget n'a pas pu être initialisée !";
        return;
    }

    chargerConfiguration();
    QString cheminLocalDossier = QDir::cleanPath(repertoireDestination);
    if (QDir().mkpath(cheminLocalDossier)) {
        qDebug() << "[SYSTEM] : Dossier de stockage local créé ou existant à :" << cheminLocalDossier;
    }

    uid_t monUid = getuid();
    QString cheminSourceUniversel = QDir::cleanPath(QString("/run/user/%1/%2").arg(monUid).arg(repertoireSource));

    if (cheminSourceUniversel.isEmpty()) {
        qDebug() << "[ERREUR CRITIQUE] : Le chemin réseau calculé est vide !";
        return;
    }

    this->connect(&this->timerApp, &QTimer::timeout, this, [this, cheminSourceUniversel]() {
        this->onDossierModifie(cheminSourceUniversel);
    });

    this->timerApp.start(3000);
    if (!this->timerApp.isActive()) {
        qDebug() << "[ATTENTION] : Le Timer de surveillance du Watchdog a échoué à démarrer !";
    } else {
        qDebug() << "[SYSTEM] : Surveillance active via config.ini sur :" << cheminSourceUniversel;
    }
}

/**
 * @brief Watchdog::~Watchdog
 * @author Harold KALO
 */
Watchdog::~Watchdog()
{
    if (this->ui != nullptr) {
        delete this->ui;
    }
}

/**
 * @brief Watchdog::chargerConfiguration
 * @author Harold KALO
 */
void Watchdog::chargerConfiguration() {
    QString pathConfig = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(pathConfig, QSettings::IniFormat);

    this->repertoireSource = settings.value("PATHS/watchdir", "/gvfs/smb-share:server=172.18.58.111,share=photossstv/").toString();
    this->repertoireDestination = settings.value("PATHS/destination", "/tmp/remote/").toString();

    this->repertoireSource.remove("\"");
    this->repertoireDestination.remove("\"");

    if (!this->repertoireSource.startsWith("/")) {
        this->repertoireSource = "/" + this->repertoireSource;
    }

    qDebug() << "[CONFIG CHASSÉE] Source brute :" << this->repertoireSource;
    qDebug() << "[CONFIG CHASSÉE] Destination brute :" << this->repertoireDestination;

}

/**
 * @brief Watchdog::onDossierModifie
 * @param path référence constante vers la chaîne contenant le chemin du dossier réseau à surveiller
 * @author Harold KALO
 */

void Watchdog::onDossierModifie (const QString &path) {
    if (path.isEmpty()) {
        qDebug() << "[ERREUR SYSTEME] : Le chemin reçu pour la surveillance est vide.";
        return;
    }
    QDir dir(path);

    QStringList filter;
    filter << "*.bmp";

    QStringList files = dir.entryList(filter, QDir::Files, QDir::Time);

    QString cheminLocalDossier = QDir::cleanPath(repertoireDestination);
    QDir().mkpath(cheminLocalDossier);

    if (!files.isEmpty()) {
        const QString nomFichier = files.first();
        if (nomFichier.isEmpty()) {
            qDebug() << "[SYSTEM] : Un fichier fantôme ou vide a été détecté.";
            return;
        }

        if (!this->imagesTraitees.contains(nomFichier)) {
            const QString completPath = QDir::cleanPath(path + "/" + nomFichier);
            const QString localPath = QDir::cleanPath(cheminLocalDossier + "/" + nomFichier);

            if (!QFile::exists(localPath)) {
                const bool copie_reussie = QFile::copy(completPath, localPath);
                if (copie_reussie) {
                    QFileDevice::Permissions permissions = QFileDevice::ReadOwner | QFileDevice::WriteOwner|QFileDevice::ReadOther;

                    if (QFile::setPermissions(localPath, permissions)) {
                        qDebug()<<"changement permission ok";
                    }
                    else
                    {
                        qDebug()<<"changement permission pas ok";
                    }

                    qDebug() << "[ARCHIVE] : Photo copiée localement dans photos_sstv";
                    qDebug() << "[NOUVEAU] : Nouvelle image détectée:" << nomFichier;

                    const bool bdd_ok = this->baseDeDonneesImages.enregistrerPhoto(nomFichier, true);
                    if (bdd_ok) {
                        qDebug() << "[SQL] : Insertion réussie pour" << nomFichier;
                        this->imagesTraitees.append(nomFichier);
                    } else {
                        qDebug() << "[ERREUR SQL] : Échec persistant pour" << nomFichier;
                    }
                } else {
                    qDebug() << "[ERREUR SYSTEME] : Échec de la copie réseau.";
                }
            }
            else
            {
                qDebug()<<localPath << "existe deja";
            }


        }
    }
}

