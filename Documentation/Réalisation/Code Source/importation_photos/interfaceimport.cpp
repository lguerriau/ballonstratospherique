#include "interfaceimport.h"
#include "ui_interfaceimport.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>

InterfaceImport::InterfaceImport(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InterfaceImport)
{
    champIP = new QLineEdit("172.18.59.60", this);
    champMdp = new QLineEdit(this);
    //champMdp->setEchoMode(QLineEdit::Password);
    champMdp->setPlaceholderText("Mot de passe");

    champDossierLocal = new QLineEdit("/home/USERS/ELEVES/CIEL2024/hkalo/sstvYoniq/ARCHIVE_PHOTOS_IMPORTATION", this);

    boutonConnexion = new QPushButton("Se connecter", this);
    boutonImporter = new QPushButton("Importer", this);

    listeFichiersVisuels = new QListWidget(this); // Pour voir les fichiers
    zoneLogs = new QTextEdit(this);
    zoneLogs->setReadOnly(true);
    zoneLogs->setFixedHeight(80); // Petite zone juste pour le statut

    // Disposition des éléments (Layout)
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    QHBoxLayout *layoutFormulaire = new QHBoxLayout();

    layoutFormulaire->addWidget(new QLabel("IP Raspberry :"));
    layoutFormulaire->addWidget(champIP);

    layoutFormulaire->addWidget(new QLabel("Mot de passe :"));
    layoutFormulaire->addWidget(champMdp);

    layoutFormulaire->addWidget(new QLabel("Dossier Local Sol :"));
    layoutFormulaire->addWidget(champDossierLocal);

    layoutPrincipal->addLayout(layoutFormulaire);
    layoutPrincipal->addWidget(boutonConnexion);
    layoutPrincipal->addWidget(new QLabel("Contenu du dossier de la Raspberry Pi :"));
    layoutPrincipal->addWidget(listeFichiersVisuels); // La liste est au centre
    layoutPrincipal->addWidget(boutonImporter);
    layoutPrincipal->addWidget(zoneLogs);

    // Connexions des boutons aux fonctions (Slots)
    connect(boutonConnexion, &QPushButton::clicked, this, &InterfaceImport::connecterEtLister);
    connect(boutonImporter, &QPushButton::clicked, this, &InterfaceImport::importerPhotos);

    this->resize(650, 450);
    this->setWindowTitle("Station Sol - Explorateur & Importation");
}

InterfaceImport::~InterfaceImport()
{
    delete ui;
}

void InterfaceImport::connecterEtLister() {
    QString ip = champIP->text();
    QString mdp = champMdp->text();

    listeFichiersVisuels->clear();
    zoneLogs->append("Connexion à " + ip + " via environnement...");

    QProcess *recupListe = new QProcess(this);

    // --- SÉCURISATION DU MOT DE PASSE VIA L'ENVIRONNEMENT ---
    // On récupère l'environnement actuel du système et on lui injecte le mot de passe
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("SSHPASS", mdp);
    recupListe->setProcessEnvironment(env);
    // ---------------------------------------------------------

    QString programme = "sshpass";
    QStringList arguments;

    // On utilise l'argument "-e" au lieu de "-p".
    // Cela dit à sshpass d'aller chercher le mot de passe dans la variable SSHPASS qu'on vient de créer.
    arguments << "-e"
              << "ssh"
              << "-o" << "StrictHostKeyChecking=no"
              << "-o" << "BatchMode=no" // On laisse "no" pour que sshpass puisse injecter le mot de passe
              << ("pbs@" + ip)
              << "ls /home/pbs/photos/";

    recupListe->start(programme, arguments);

    if (recupListe->waitForFinished(10000)) {
        if (recupListe->exitCode() == 0) {
            QString sortie = QString::fromUtf8(recupListe->readAllStandardOutput()).trimmed();
            if (!sortie.isEmpty()) {
                QStringList fichiers = sortie.split("\n");
                listeFichiersVisuels->addItems(fichiers);
                zoneLogs->append("✅ Dossier mis à jour.");
            } else {
                zoneLogs->append("✅ Connecté, mais aucun fichier dans le dossier.");
            }
        } else {
            QString erreur = QString::fromUtf8(recupListe->readAllStandardError());
            zoneLogs->append("❌ Erreur (" + QString::number(recupListe->exitCode()) + ") : " + erreur);
        }
    } else {
        zoneLogs->append("⏳ Le délai d'attente a expiré.");
        recupListe->kill();
    }
    recupListe->deleteLater();
}

void InterfaceImport::importerPhotos() {
    QString ip = champIP->text();
    QString mdp = champMdp->text();
    QString destLocal = champDossierLocal->text();

    zoneLogs->append("Importation des photos via environnement...");

    QDir dir(destLocal);
    if (!dir.exists()) dir.mkpath(".");

    QProcess *copieDossier = new QProcess(this);

    // --- SÉCURISATION DU MOT DE PASSE VIA L'ENVIRONNEMENT ---
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("SSHPASS", mdp);
    copieDossier->setProcessEnvironment(env);
    // ---------------------------------------------------------

    QString programme = "sshpass";
    QStringList arguments;

    arguments << "-e"
              << "scp"
              << "-o" << "StrictHostKeyChecking=no"
              << ("pbs@" + ip + ":/home/pbs/photos/*.jpg")
              << destLocal;

    copieDossier->start(programme, arguments);

    if (copieDossier->waitForFinished(20000)) {
        if (copieDossier->exitCode() == 0) {
            zoneLogs->append("📥 Importation réussie dans " + destLocal);
        } else {
            QString erreur = QString::fromUtf8(copieDossier->readAllStandardError());
            zoneLogs->append("❌ Échec : " + erreur);
        }
    } else {
        zoneLogs->append("⏳ Transfert expiré.");
        copieDossier->kill();
    }
    copieDossier->deleteLater();
}
