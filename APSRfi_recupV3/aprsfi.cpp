#include "aprsfi.h"
#include "ui_aprsfi.h"
#include <QDateTime>
#include <QApplication>
#include <QFile>

aprsfi::aprsfi(QWidget *parent) : QWidget(parent), ui(new Ui::aprsfi) {
    ui->setupUi(this);

    // Initialisation des modules
    database = new DatabaseManager(this);
    apiClient = new ApiClient(this);
    wsServer = new WebSocketServer(this);

    // Connexions simples
    connect(database, &DatabaseManager::logMessage, this, &aprsfi::onLogMessage);
    connect(apiClient, &ApiClient::logMessage, this, &aprsfi::onLogMessage);
    connect(apiClient, &ApiClient::dataReceived, this, &aprsfi::onApiDataReceived);
    connect(apiClient, &ApiClient::rawResponse, this, &aprsfi::onApiRawResponse);
    connect(wsServer, &WebSocketServer::logMessage, this, &aprsfi::onLogMessage);

    loadSettings();
    logToUI("Application prete.");
    updateButtons();
}

aprsfi::~aprsfi() { delete ui; }

void aprsfi::loadSettings() {
    // Chemin absolu basé sur votre capture d'écran
    QString configPath = "/home/USERS/ELEVES/CIEL2024/lguerriau/ProjetPBS/APSRFi_recupV3/config.ini";

    QSettings settings(configPath, QSettings::IniFormat);

    config.callsign = settings.value("API/name", "OH7RDA").toString();
    config.apiKey = settings.value("API/apikey").toString();
    config.interval = settings.value("API/interval", 60000).toInt();

    // Récupération des infos de base de données (127.0.0.1 évite le bug du localhost)
    config.dbHost = settings.value("Database/host", "127.0.0.1").toString();
    config.dbUser = settings.value("Database/username", "root").toString();
    config.dbPass = settings.value("Database/password", "").toString();
    config.dbName = settings.value("Database/database", "votre_base").toString();

    config.wsPort = settings.value("WebSocket/port", 12345).toInt();

    // Vérification dans les logs de l'interface
    if (QFile::exists(configPath)) {
        logToUI("Configuration chargee depuis : " + configPath);
        logToUI("Utilisateur BDD lu : " + config.dbUser);
    } else {
        logToUI("ATTENTION : Le fichier config.ini est introuvable !", true);
    }
}

void aprsfi::on_LancerServeur_clicked() {
    // UTILISATION DES VARIABLES DU FICHIER INI ICI (et plus "root", "toto")
    if (!database->connect(config.dbHost, config.dbUser, config.dbPass, config.dbName)) return;

    apiClient->configure(config.callsign, "loc", config.apiKey, config.interval);
    apiClient->startPolling();
    wsServer->start(config.wsPort);

    isRunning = true;
    updateButtons();
    logToUI("Le backend est maintenant actif.");
}

void aprsfi::on_ForcerAPI_clicked() {
    logToUI("Demande de mise a jour manuelle.");
    apiClient->fetchNow();
}

void aprsfi::on_Quitter_clicked() { QApplication::quit(); }

void aprsfi::onApiDataReceived(const QJsonArray &entries) {
    for (const QJsonValue &val : entries) database->saveEntry(val.toObject());
    wsServer->broadcastPositions(database->getCurrentPositions());
}

void aprsfi::onApiRawResponse(const QString &json) {
    ui->RepAPI->setPlainText(json);
}

void aprsfi::logToUI(const QString &message, bool isError) {
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString prefix = isError ? "[ERREUR]" : "[INFO]";
    ui->LogsProgramme->append(QString("[%1] %2 %3").arg(time, prefix, message));
}

void aprsfi::onLogMessage(const QString &msg) { logToUI(msg); }
void aprsfi::onErrorMessage(const QString &err) { logToUI(err, true); }

void aprsfi::updateButtons() {
    ui->LancerServeur->setEnabled(!isRunning);
    ui->ForcerAPI->setEnabled(isRunning);
}
