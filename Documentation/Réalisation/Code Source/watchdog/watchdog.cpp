#include "watchdog.h"
#include "ui_watchdog.h"
#include <QDir>
#include <QDebug>
#include <QTimer>

Watchdog::Watchdog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 1. On initialise ta classe SQL
    maSql = new Bdd();

    // 2. On remplace le Watchdog par un Timer (plus fiable sur réseau SMB)
    QTimer *timer = new QTimer(this);

    // On définit le chemin exact que tu as validé avec 'ls'
    // Note : On ajoute un "/" à la fin pour être sûr du chemin
    QString chemin = "/run/user/2094/gvfs/smb-share:server=172.18.58.111,share=photossstv/";

    // 3. Connexion : On demande au timer d'appeler ta fonction toutes les 3 secondes
    connect(timer, &QTimer::timeout, this, [this, chemin]() {
        this->onDossierModifie(chemin);
    });

    // On lance le chrono !
    timer->start(3000);

    qDebug() << "[SYSTEM] : Surveillance active sur" << chemin;
}

Watchdog::~Watchdog()
{
    delete ui;
}



void Watchdog::onDossierModifie(const QString &path) {
    QDir dir(path);
    QStringList filter;
    filter << "*.jpg" << "*.JPG" << "*.jpeg" << "*.JPEG"<< "*.bmp";

    QStringList files = dir.entryList(filter, QDir::Files, QDir::Time);

    if (!files.isEmpty()) {
        QString nomFichier = files.first(); // On prend la plus récente
        QString completPath = path + nomFichier;

        QString localPath = "/home/USERS/ELEVES/CIEL2024/hkalo/sstvYoniq/ARCHIVE_PHOTOS/" + nomFichier;
        if (!QFile::exists(localPath)) {
                    if (QFile::copy(completPath, localPath)) {
                        qDebug() << "[ARCHIVE] : Photo copiée localement dans ARCHIVE_PHOTOS";
                    }
                }

        // --- LA PROTECTION CONTRE LES DOUBLONS ---
        if (imagesTraitees.contains(nomFichier)) {
            // On a déjà cette image, on ne fait rien
            return;
        }
        // Si on arrive ici, c'est que l'image est NOUVELLE
        qDebug() << "[NOUVEAU] : Nouvelle image détectée :" << nomFichier;

        if (maSql->enregistrerPhoto(completPath, true)) {
            qDebug() << "[SQL] : Insertion réussie pour" << nomFichier;

            // ON AJOUTE L'IMAGE À NOTRE MÉMOIRE
            imagesTraitees.append(nomFichier);
        } else {
            qDebug() << "[ERREUR SQL] : Impossible d'insérer" << nomFichier;
        }
    }
}
