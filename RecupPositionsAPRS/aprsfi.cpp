#include "aprsfi.h"
#include "ui_aprsfi.h"
#include <QDateTime>
#include <QApplication>
#include <QFile>
#include <QCoreApplication>
#include <QDir>

aprsfi::aprsfi(QWidget *parent) : QWidget(parent), ui(new Ui::aprsfi) {
    ui->setupUi(this);

    database = new DatabaseManager(this);
    apiClient = new ApiClient(this);
    wsServer = new WebSocketServer(this);

    connect(database, &DatabaseManager::logMessage, this, &aprsfi::onLogMessage);
    connect(database, &DatabaseManager::errorOccurred, this, &aprsfi::onErrorMessage);

    connect(apiClient, &ApiClient::logMessage, this, &aprsfi::onLogMessage);
    connect(apiClient, &ApiClient::errorOccurred, this, &aprsfi::onErrorMessage);
    connect(apiClient, &ApiClient::dataReceived, this, &aprsfi::onApiDataReceived);
    connect(apiClient, &ApiClient::rawResponse, this, &aprsfi::onApiRawResponse);

    connect(wsServer, &WebSocketServer::logMessage, this, &aprsfi::onLogMessage);
    connect(wsServer, &WebSocketServer::errorOccurred, this, &aprsfi::onErrorMessage);

    // --- LECTURE DU FICHIER INI ---
    QString configPath = "/home/USERS/ELEVES/CIEL2024/lguerriau/ProjetPBS/RecupPositionsAPRS/config.ini";
    settings = new QSettings(configPath, QSettings::IniFormat, this);

    // On charge les variables au démarrage de l'app
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
    if (isRunning) return;

    logToUI("Demarrage du serveur avec les parametres du config.ini...");

    // Lancement de la connexion avec les variables (qui sont déjà chargées)
    if (!database->connect(dbHost, dbUser, dbPass, dbName)) {
        logToUI("ERREUR FATALE : Impossible de se connecter a la base de donnees.", true);
        return;
    }

    wsServer->start(wsPort);
    apiClient->configure(apiName, apiWhat, apiKey, apiInterval);
    apiClient->startPolling();

    isRunning = true;
    updateButtons();
}

void aprsfi::on_ForcerAPI_clicked() {
    logToUI("Demande de mise a jour manuelle API...");
    ui->RepAPI->clear();
    ui->RepAPI->append("En attente de reponse...");
    apiClient->fetchNow();
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

void aprsfi::onLogMessage(const QString &msg) { logToUI(msg); }
void aprsfi::onErrorMessage(const QString &err) { logToUI(err, true); }

void aprsfi::updateButtons() {
    ui->LancerServeur->setEnabled(!isRunning);
    ui->ForcerAPI->setEnabled(isRunning);
}
