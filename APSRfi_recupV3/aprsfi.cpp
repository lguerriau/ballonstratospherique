#include "aprsfi.h"
#include "ui_aprsfi.h"
#include <QDateTime>
#include <QApplication>

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
    QSettings settings("/home/USERS/ELEVES/CIEL2024/lguerriau/ProjetPBS/APRSfi_recup/config.ini", QSettings::IniFormat);

    config.callsign = settings.value("API/name", "OH7RDA").toString();
    config.apiKey = settings.value("API/apikey").toString();
    config.interval = settings.value("API/interval", 60000).toInt();
    config.dbHost = settings.value("Database/host").toString();
    config.dbName = settings.value("Database/database").toString();
    config.wsPort = settings.value("WebSocket/port", 12345).toInt();

    logToUI("Configuration chargee.");
}

void aprsfi::on_LancerServeur_clicked() {
    if (!database->connect(config.dbHost, "root", "toto", config.dbName)) return;

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
    QString prefix = isError ? "[ERREUR]" : "[INFO]"; // Utilisation du paramètre
    ui->LogsProgramme->append(QString("[%1] %2 %3").arg(time, prefix, message));
}

void aprsfi::onLogMessage(const QString &msg) { logToUI(msg); }
void aprsfi::onErrorMessage(const QString &err) { logToUI(err, true); }

void aprsfi::updateButtons() {
    ui->LancerServeur->setEnabled(!isRunning);
    ui->ForcerAPI->setEnabled(isRunning);
}
