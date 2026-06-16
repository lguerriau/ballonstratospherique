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

    // Le chemin réseau de la station au sol Windows (172.18.58.111)
    QString runtimeDir = qEnvironmentVariable("XDG_RUNTIME_DIR");
    const QString chemin = runtimeDir + repertoireSource;

    if (chemin.isEmpty()) {
        qDebug() << "[ERREUR CRITIQUE] : Le chemin réseau calculé est vide !";
        return;
    }

    this->connect(&this->timerApp, &QTimer::timeout, this, [this, chemin]() {
        this->onDossierModifie(chemin);
    });

    this->timerApp.start(3000);
    if (!this->timerApp.isActive()) {
        qDebug() << "[ATTENTION] : Le Timer de surveillance du Watchdog a échoué à démarrer !";
    } else {
        qDebug() << "[SYSTEM] : Surveillance active sur" << chemin;
    }

    qDebug() << "[SYSTEM] : Surveillance active sur" << chemin;
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
    QSettings settings("config.ini", QSettings::IniFormat);

    this->repertoireSource = settings.value("PATHS/watchdir", "toto").toString();
    this->repertoireDestination = settings.value("PATHS/destination", "titi").toString();

    qDebug()<<"source : "<<this->repertoireSource;
    qDebug()<<"dst : "<<this->repertoireDestination;
}


/**
 * @brief Watchdog::onDossierModifie
 * @param path référence constante vers la chaîne contenant le chemin du dossier réseau à surveiller
 * @author Harold KALO
 */
void Watchdog::onDossierModifie(const QString &path) {
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
