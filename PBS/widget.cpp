#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 1. On s'assure que la table a bien 3 colonnes
    ui->table_requetes->setColumnCount(3);

    // 2. On configure la colonne du milieu (index 1) pour qu'elle s'étire
    ui->table_requetes->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->table_requetes->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->table_requetes->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);

    // 3. (Optionnel) On donne une taille fixe aux deux autres
    ui->table_requetes->setColumnWidth(0, 150);
    ui->table_requetes->setColumnWidth(2, 150);
    // 1. Instanciation de notre nouvelle classe métier
    lora = new CommunicationLora(this);

    // 2. Connexion du signal (quand la classe LoRa reçoit un texte) vers notre slot (pour l'afficher)
    connect(lora, &CommunicationLora::messageRecu, this, &Widget::lireDonneesSerie);

    // 3. Ouverture du port série
    if (lora->ouvrirPort("/dev/ttyACM0")) {
        qDebug() << "Port ouvert avec succès.";
        ui->lbl_image_principale->setText("[ SYSTÈME CONNECTÉ ]");
    } else {
        qDebug() << "Échec de l'ouverture du port.";
        ui->lbl_image_principale->setText("[ ERREUR DE CONNEXION AU PORT ]");
    }
}

Widget::~Widget()
{
    // Nettoyage de la mémoire à la fermeture
    delete ui;
    // Note: 'lora' est automatiquement détruit car on a passé 'this' en parent
}

void Widget::on_btn_envoyer_clicked()
{
    QString commandeSelectionnee = ui->combo_requetes->currentText();

    // Si on a sélectionné la récupération RSSI/SNR
    if (commandeSelectionnee == "RÉCUPÉRER RSSI & SNR") {

        // On délègue l'envoi à notre classe métier
        lora->demandeRssiSnr();

        // On démarre le chronomètre pour calculer la latence
        timerLatence.start();

        // On ajoute une ligne dans le journal de bord (Tableau)
        int row = ui->table_requetes->rowCount();
        ui->table_requetes->insertRow(row);

        // Colonne 0 : Heure
        ui->table_requetes->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString("HH:mm:ss")));

        // Colonne 1 : Texte de la requête
        ui->table_requetes->setItem(row, 1, new QTableWidgetItem("REQ: " + commandeSelectionnee));

        // Colonne 2 : En attente...
        ui->table_requetes->setItem(row, 2, new QTableWidgetItem("..."));
    }
}

void Widget::lireDonneesSerie(const QString &message)
{
    // Ce slot est appelé AUTOMATIQUEMENT quand la classe CommunicationLora émet "messageRecu"

    // 1. Mise à jour du tableau avec la réponse
    int lastRow = ui->table_requetes->rowCount() - 1;
    if (lastRow >= 0) {
        // On calcule le temps écoulé depuis le clic
        qint64 ms = timerLatence.elapsed();

        // Remplacement des '|' par des espaces pour un affichage plus joli dans le tableau
        QString affichageTableau = message;
        affichageTableau.replace("|", " | ");

        ui->table_requetes->setItem(lastRow, 1, new QTableWidgetItem("RES: " + affichageTableau));
        ui->table_requetes->setItem(lastRow, 2, new QTableWidgetItem(QString::number(ms)));
    }
}
