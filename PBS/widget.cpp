#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QMessageBox>

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


    // --- OUVERTURE DU PORT (RETOUR AU ACM0) ---
        if (lora->ouvrirPort("/dev/ttyACM0")) {
            qDebug() << "✅ Port /dev/ttyACM0 ouvert !";

            // On débloque le bandeau de chargement immédiatement
            ui->lbl_status_vol->setProperty("status", "LANDING"); // Devient Vert
            ui->lbl_status_vol->setText("SYSTÈME CONNECTÉ - PRÊT");
        } else {
            qDebug() << "❌ ÉCHEC : /dev/ttyACM0 introuvable ou verrouillé.";

            ui->lbl_status_vol->setProperty("status", "BURST"); // Devient Rouge
            ui->lbl_status_vol->setText("ERREUR PORT : /dev/ttyACM0");
        }

        // Rafraîchissement du style pour que la couleur change bien
        ui->lbl_status_vol->style()->unpolish(ui->lbl_status_vol);
        ui->lbl_status_vol->style()->polish(ui->lbl_status_vol);
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
    // Nettoyage de la chaîne reçue (enlève les espaces et retours à la ligne)
    QString msg = message.trimmed();

    // 1. DÉTECTION DES FLAGS DE VOL (Déjà fait)
    if (msg.startsWith("ST:")) {
        QString flag = msg.split(":").last();
        updateFlightStatus(flag);

        // Log dans le tableau
        int row = ui->table_requetes->rowCount();
        ui->table_requetes->insertRow(row);
        ui->table_requetes->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString("HH:mm:ss")));
        ui->table_requetes->setItem(row, 1, new QTableWidgetItem("ALERTE : " + flag));
        return;
    }

    // 2. DÉTECTION DE L'ACCÉLÉRATION
    if (msg.startsWith("Z:")) {
        QString accel = msg.split(":").last();
        // Note: Tu n'as pas de label "lbl_val_accel" dans ton .ui pour le moment.
        // Tu peux soit l'ajouter sur Qt Designer, soit l'afficher dans les logs :
        qDebug() << "Accélération Z reçue :" << accel << "g";
        return;
    }
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

void Widget::updateFlightStatus(const QString &status)
{
    // 1. On change la propriété dynamique (utilisée par le QSS)
    ui->lbl_status_vol->setProperty("status", status);

    // 2. On force le rafraîchissement du style (Astuce indispensable)
    ui->lbl_status_vol->style()->unpolish(ui->lbl_status_vol);
    ui->lbl_status_vol->style()->polish(ui->lbl_status_vol);

    // 3. On change uniquement le texte et les alertes bloquantes
    if (status == "BURST") {
        ui->lbl_status_vol->setText("⚠️ ÉCLATEMENT BALLON - CHUTE EN COURS ⚠️");
        QMessageBox::critical(this, "ALERTE", "Le capteur a détecté un BURST !");
    }
    else if (status == "LANDING") {
        ui->lbl_status_vol->setText("✅ NACELLE AU SOL");
    }
    else if (status == "en vol") {
        ui->lbl_status_vol->setText("🚀 ASCENSION EN COURS");
    }
}
