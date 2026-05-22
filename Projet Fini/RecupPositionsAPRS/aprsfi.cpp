/**
 * @file aprsfi.cpp
 * @brief Implémentation de la classe aprsfi
 * @details Fenêtre principale — orchestre l'API, la base de données et le WebSocket
 * @version 4.0
 * @date 22/05/2026
 * @author Guerriau Lucien
 */

#include "aprsfi.h"
#include "ui_aprsfi.h"
#include <QDateTime>
#include <QApplication>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QTimer>

/**
 * @brief Constructeur — initialise les composants et charge la configuration
 * @details Instancie DatabaseManager, ApiClient (x2) et WebSocketServer,
 *          connecte tous les signaux, puis charge config.ini.
 * @param parent Widget parent Qt
 */
aprsfi::aprsfi(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::aprsfi)
    , database(nullptr)
    , apiClient(nullptr)
    , wsServer(nullptr)
    , isRunning(false)
    , settings(nullptr)
    , apiInterval(0) {

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
    connect(apiClientWx, &ApiClient::dataReceived, this, &aprsfi::onApiDataReceived);
    connect(apiClientWx, &ApiClient::rawResponse, this, &aprsfi::onApiRawResponse);

    connect(wsServer, &WebSocketServer::logMessage, this, &aprsfi::onLogMessage);
    connect(wsServer, &WebSocketServer::errorOccurred, this, &aprsfi::onErrorMessage);

    QString configPath = "config.ini";
    settings = new QSettings(configPath, QSettings::IniFormat, this);

    loadSettings();

    logToUI("Application prete. Fichier cible : " + configPath);
    updateButtons();
}

/**
 * @brief Destructeur
 */
aprsfi::~aprsfi() {
    delete ui;
}

/**
 * @brief Charge tous les paramètres depuis config.ini
 * @details Lit la section [API] et délègue loadConfig à DatabaseManager et WebSocketServer
 */
void aprsfi::loadSettings() {
    apiName = settings->value("API/name").toString();
    apiWhat = settings->value("API/what").toString();
    apiWhatWx = settings->value("API/what_wx").toString();
    apiKey = settings->value("API/apikey").toString();
    apiFormat = settings->value("API/format").toString();
    apiInterval = settings->value("API/interval").toInt();

    database->loadConfig(settings);
    wsServer->loadConfig(settings);
}

/**
 * @brief Slot du bouton "Lancer le serveur"
 * @details Enchaîne : connexion BDD → démarrage WebSocket → démarrage polling API (loc immédiat,
 *          wx décalé de 5 secondes pour éviter la surcharge initiale).
 *          Désactive le bouton une fois le serveur démarré.
 */
void aprsfi::on_LancerServeur_clicked() {
    if (!isRunning) {
        logToUI("Demarrage du serveur avec les parametres du config.ini...");
        DbStatus dbStatus = database->connectToDatabase();

        if (dbStatus == DB_SUCCESS) {
            WsStatus wsStatus = wsServer->start();

            if (wsStatus == WS_SUCCESS) {
                apiClient->configure(apiName, apiWhat, apiKey, apiInterval);
                apiClient->startPolling();

                apiClientWx->configure(apiName, apiWhatWx, apiKey, apiInterval);
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

/**
 * @brief Slot du bouton "Forcer l'API"
 * @details Déclenche immédiatement une requête sur les deux clients API.
 *          La requête wx est décalée de 2 secondes pour éviter la collision de réponses.
 */
void aprsfi::on_ForcerAPI_clicked() {
    logToUI("Demande de mise a jour manuelle API (Loc + Wx)...");
    ui->RepAPI->clear();
    ui->RepAPI->append("En attente de reponse...");

    apiClient->sendRequest();
    QTimer::singleShot(2000, apiClientWx, &ApiClient::sendRequest);
}

/**
 * @brief Slot de réception des données API parsées
 * @details Redirige les données vers saveTelemetry (type wx) ou saveEntry (type loc),
 *          puis déclenche un broadcast WebSocket des positions mises à jour.
 * @param entries Tableau JSON des entrées reçues
 * @param type Type de données reçu, comparé à apiWhatWx pour distinguer loc et wx
 */
void aprsfi::onApiDataReceived(const QJsonArray &entries, const QString &type) {
    if (type == apiWhatWx) {
        for (const QJsonValue &val : entries) {
            database->saveTelemetry(val.toObject());
        }
    } else {
        for (const QJsonValue &val : entries) {
            database->saveEntry(val.toObject());
        }
    }
    wsServer->broadcastPositions(database->getCurrentPositions());
}

/**
 * @brief Slot de réception de la réponse brute JSON
 * @details Affiche la réponse dans le panneau RepAPI précédée d'un en-tête
 *          indiquant le type de données (LOC ou WX).
 * @param json Réponse JSON brute formatée
 * @param type Type de données pour sélectionner l'en-tête affiché
 */
void aprsfi::onApiRawResponse(const QString &json, const QString &type) {
    if (type == apiWhatWx) {
        ui->RepAPI->append("\n================== TELEMETRIES (WX) ==================\n");
    } else {
        ui->RepAPI->append("\n================== POSITIONS (LOC) ==================\n");
    }
    ui->RepAPI->append(json);
}

/**
 * @brief Affiche un message horodaté dans le panneau de logs
 * @param message Texte à afficher
 * @param isError Si true, le message est préfixé [ERREUR], sinon [INFO]
 */
void aprsfi::logToUI(const QString &message, bool isError) {
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString prefix = isError ? "[ERREUR]" : "[INFO]";
    ui->LogsProgramme->append(QString("[%1] %2 %3").arg(time, prefix, message));
}

/**
 * @brief Slot de réception des messages de log des composants
 * @param msg Texte informatif à afficher
 */
void aprsfi::onLogMessage(const QString &msg) {
    logToUI(msg, false);
}

/**
 * @brief Slot de réception des messages d'erreur des composants
 * @param err Texte d'erreur à afficher
 */
void aprsfi::onErrorMessage(const QString &err) {
    logToUI(err, true);
}

/**
 * @brief Met à jour l'état actif/inactif des boutons selon isRunning
 * @details LancerServeur est désactivé une fois le serveur lancé ;
 *          ForcerAPI est activé uniquement quand le serveur tourne.
 */
void aprsfi::updateButtons() {
    ui->LancerServeur->setEnabled(!isRunning);
    ui->ForcerAPI->setEnabled(isRunning);
}
