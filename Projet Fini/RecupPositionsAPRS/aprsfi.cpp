#include "aprsfi.h"
#include "ui_aprsfi.h"
#include <QDateTime>
#include <QApplication>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QTimer>

aprsfi::aprsfi(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::aprsfi)
    , database(nullptr)
    , apiClient(nullptr)
    , wsServer(nullptr)
    , isRunning(false)
    , settings(nullptr)
    , apiInterval(0)
    , wsPort(0) {

    ui->setupUi(this);

    database = new DatabaseManager(this);
    apiClient = new ApiClient(this);
    apiClientWx = new ApiClient(this);
    wsServer = new WebSocketServer(this);

    connect(database, &DatabaseManager::logMessage, this, &aprsfi::onLogMessage);
    connect(database, &DatabaseManager::errorOccurred, this, &aprsfi::onErrorMessage);

    connect(apiClient, &ApiClient::logMessage, this, &aprsfi::onLogMessage);
    connect(apiClient, &ApiClient::errorOccurred, this, &aprsfi::onErrorMessage);
    connect(apiClient, &ApiClient::dataReceived, this, &aprsfi::onApiDataReceived);
    connect(apiClient, &ApiClient::rawResponse, this, &aprsfi::onApiRawResponse);

    connect(apiClientWx, &ApiClient::logMessage, this, &aprsfi::onLogMessage);
    connect(apiClientWx, &ApiClient::errorOccurred, this, &aprsfi::onErrorMessage);
    connect(apiClientWx, &ApiClient::dataReceived, this, &aprsfi::onApiDataReceivedWx);
    connect(apiClientWx, &ApiClient::rawResponse, this, &aprsfi::onApiRawResponseWx);

    connect(wsServer, &WebSocketServer::logMessage, this, &aprsfi::onLogMessage);
    connect(wsServer, &WebSocketServer::errorOccurred, this, &aprsfi::onErrorMessage);

    QString configPath = "/home/USERS/ELEVES/CIEL2024/lguerriau/ProjetPBS/RecupPositionsAPRS/config.ini";
    settings = new QSettings(configPath, QSettings::IniFormat, this);

    loadSettings();

    logToUI("Application prete. Fichier cible : " + configPath);
    updateButtons();
}

aprsfi::~aprsfi() {
    delete ui;
}

void aprsfi::loadSettings() {
    apiName = settings->value("API/name").toString();
    apiWhat = settings->value("API/what").toString();
    apiWhatWx = settings->value("API/what_wx").toString();
    apiKey = settings->value("API/apikey").toString();
    apiFormat = settings->value("API/format").toString();
    apiInterval = settings->value("API/interval").toInt();

    dbHost = settings->value("Database/host").toString();
    dbUser = settings->value("Database/username").toString();
    dbPass = settings->value("Database/password").toString();
    dbName = settings->value("Database/database").toString();

    wsPort = settings->value("WebSocket/port").toInt();
}

void aprsfi::on_LancerServeur_clicked() {
    if (!isRunning) {
        logToUI("Demarrage du serveur avec les parametres du config.ini...");
        DbStatus dbStatus = database->connectToDatabase(dbHost, dbUser, dbPass, dbName);

        if (dbStatus == DB_SUCCESS) {
            WsStatus wsStatus = wsServer->start(wsPort);

            if (wsStatus == WS_SUCCESS) {
                apiClient->configure(apiName, apiWhat, apiKey, apiInterval);
                apiClient->startPolling(); // Démarre immédiatement

                apiClientWx->configure(apiName, apiWhatWx, apiKey, apiInterval);
                // Décalage de 5 secondes (5000 ms) pour la télémétrie afin d'éviter le blocage de l'API (Rate Limit)
                QTimer::singleShot(5000, apiClientWx, &ApiClient::startPolling);

                isRunning = true;
                updateButtons();
            } else {
                logToUI("ERREUR : Impossible de demarrer le serveur WebSocket.", true);
            }
        } else {
            logToUI("ERREUR FATALE : Impossible de se connecter a la base de donnees.", true);
        }
    }
}

void aprsfi::on_ForcerAPI_clicked() {
    logToUI("Demande de mise a jour manuelle API (Loc + Wx)...");
    ui->RepAPI->clear();
    ui->RepAPI->append("En attente de reponse...");

    apiClient->sendRequest();

    // Décalage de 2 secondes en manuel pour ne pas heurter la limite de requêtes simultanées
    QTimer::singleShot(2000, apiClientWx, &ApiClient::sendRequest);
}

void aprsfi::on_Quitter_clicked() {
    QApplication::quit();
}

void aprsfi::onApiDataReceived(const QJsonArray &entries) {
    for (const QJsonValue &val : entries) {
        database->saveEntry(val.toObject());
    }
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

void aprsfi::onLogMessage(const QString &msg) {
    logToUI(msg, false);
}

void aprsfi::onErrorMessage(const QString &err) {
    logToUI(err, true);
}

void aprsfi::updateButtons() {
    ui->LancerServeur->setEnabled(!isRunning);
    ui->ForcerAPI->setEnabled(isRunning);
}

void aprsfi::onApiDataReceivedWx(const QJsonArray &entries) {
    for (const QJsonValue &val : entries) {
        database->saveTelemetry(val.toObject());
    }
    // NOUVELLE LIGNE : On pousse la mise à jour globale vers le site web
    wsServer->broadcastPositions(database->getCurrentPositions());
}

void aprsfi::onApiRawResponseWx(const QString &json) {
    // On ajoute un séparateur visuel clair suivi du flux JSON de la météo
    ui->RepAPI->append("\n================== TELEMETRIE (WX) ==================\n");
    ui->RepAPI->append(json);
}
