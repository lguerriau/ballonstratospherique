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

    assert(this->ui != nullptr);

    // Le chemin réseau de la station au sol Windows (172.18.58.111)
    const QString chemin = "/run/user/2094/gvfs/smb-share:server=172.18.58.111,share=photossstv/";
    assert(!chemin.isEmpty());

    this->connect(&this->timerApp, &QTimer::timeout, this, [this, chemin]() {
        this->onDossierModifie(chemin);
    });

    // Fréquence d'interrogation du Watchdog fixée à 3 secondes
    this->timerApp.start(3000);
    assert(this->timerApp.isActive());

    qDebug() << "[SYSTEM] : Surveillance active sur" << chemin;
}

/**
 * @brief Watchdog::~Watchdog
 * @author Harold KALO
 */
Watchdog::~Watchdog()
{
    assert(this->ui != nullptr);
    delete this->ui;
}

/**
 * @brief Watchdog::onDossierModifie
 * @param path référence constante vers la chaîne contenant le chemin du dossier réseau à surveiller
 * @author Harold KALO
 */
void Watchdog::onDossierModifie(const QString &path) {
    assert(!path.isEmpty());

    QDir dir(path);
    QStringList filter;
    filter << "*.bmp";

    QStringList files = dir.entryList(filter, QDir::Files, QDir::Time);
    const QString appDir = QCoreApplication::applicationDirPath();

    if (!files.isEmpty()) {
        const QString nomFichier = files.first();
        assert(!nomFichier.isEmpty());

        if (!this->imagesTraitees.contains(nomFichier)) {
            const QString completPath = path + "/" + nomFichier;
            const QString localPath = appDir + "/ARCHIVE_PHOTOS/" + nomFichier;
            if (!QFile::exists(localPath)) {
                const bool copie_reussie = QFile::copy(completPath, localPath);
                if (copie_reussie) {
                    qDebug() << "[ARCHIVE] : Photo copiée localement dans ARCHIVE_PHOTOS";
                } else {
                    qDebug() << "[ERREUR SYSTEME] : Échec de la copie réseau.";
                }
            }

            qDebug() << "[NOUVEAU] : Nouvelle image détectée :" << nomFichier;

            const bool bdd_ok = this->maSql.enregistrerPhoto(completPath, true);
            if (bdd_ok) {
                qDebug() << "[SQL] : Insertion réussie pour" << nomFichier;
                this->imagesTraitees.append(nomFichier);
            } else {
                qDebug() << "[ERREUR SQL] : Échec persistant pour" << nomFichier;
            }
        }
    }
}
