#include "aprsfi.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QApplication>
#include <QJsonArray>

aprsfi::aprsfi(QWidget *parent) :
    QWidget(parent),
    isRunning(false)
{
    // Interface graphique
    setupUI();

    // Chargement de la configuration
    QString configPath = "/home/USERS/ELEVES/CIEL2024/lguerriau/ProjetPBS/APRSfi_recup/config.ini";
    settings = new QSettings(configPath, QSettings::IniFormat, this);
    loadSettings();

    // Création des 3 modules principaux
    database = new DatabaseManager(this);
    apiClient = new ApiClient(this);
    wsServer = new WebSocketServer(this);

    // ========================================
    // Connexion des signaux/slots entre modules
    // ========================================

    // Logs de la BDD → UI
    connect(database, &DatabaseManager::logMessage, this, &aprsfi::onLogMessage);
    connect(database, &DatabaseManager::errorOccurred, this, &aprsfi::onErrorMessage);

    // Logs de l'API → UI
    connect(apiClient, &ApiClient::logMessage, this, &aprsfi::onLogMessage);
    connect(apiClient, &ApiClient::errorOccurred, this, &aprsfi::onErrorMessage);
    connect(apiClient, &ApiClient::dataReceived, this, &aprsfi::onApiDataReceived);
    connect(apiClient, &ApiClient::rawResponse, this, &aprsfi::onApiRawResponse);

    // Logs du WebSocket → UI
    connect(wsServer, &WebSocketServer::logMessage, this, &aprsfi::onLogMessage);
    connect(wsServer, &WebSocketServer::errorOccurred, this, &aprsfi::onErrorMessage);

    logToUI("Application initialisée. Prêt à démarrer !");
    updateStatusLabel();
}

aprsfi::~aprsfi() {
    // Nettoyage automatique par Qt (parents/enfants)
}

// ==========================================
// INTERFACE GRAPHIQUE
// ==========================================

void aprsfi::setupUI() {
    setWindowTitle("APRS Backend - Dashboard");
    resize(1000, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // === Barre de statut en haut ===
    statusLabel = new QLabel("Arrêté", this);
    statusLabel->setStyleSheet(
        "background-color: #f0f0f0; "
        "padding: 8px; "
        "border-radius: 4px; "
        "font-weight: bold;"
    );
    mainLayout->addWidget(statusLabel);

    // === Boutons de contrôle ===
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    btnStart = new QPushButton("Démarrer le Backend", this);
    btnStart->setStyleSheet("background-color: #4CAF50; color: white; padding: 10px; font-weight: bold;");

    btnForce = new QPushButton("Forcer une requête API", this);
    btnForce->setEnabled(false);

    btnQuit = new QPushButton("Quitter", this);
    btnQuit->setStyleSheet("background-color: #f44336; color: white; padding: 10px;");

    buttonLayout->addWidget(btnStart);
    buttonLayout->addWidget(btnForce);
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnQuit);

    mainLayout->addLayout(buttonLayout);

    // === Terminaux de log ===
    QHBoxLayout *terminalLayout = new QHBoxLayout();

    // Terminal des logs système
    QVBoxLayout *logLayout = new QVBoxLayout();
    QLabel *logLabel = new QLabel("Logs Système", this);
    logLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    logLayout->addWidget(logLabel);

    logTerminal = new QTextEdit(this);
    logTerminal->setReadOnly(true);
    logTerminal->setStyleSheet(
        "background-color: #1e1e1e; "
        "color: #d4d4d4; "
        "font-family: 'Courier New', monospace; "
        "font-size: 12px; "
        "border: 1px solid #444;"
    );
    logLayout->addWidget(logTerminal);

    // Terminal de la réponse API
    QVBoxLayout *apiLayout = new QVBoxLayout();
    QLabel *apiLabel = new QLabel("Réponse API (JSON)", this);
    apiLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    apiLayout->addWidget(apiLabel);

    apiTerminal = new QTextEdit(this);
    apiTerminal->setReadOnly(true);
    apiTerminal->setStyleSheet(
        "background-color: #f9f9f9; "
        "color: #333; "
        "font-family: 'Courier New', monospace; "
        "font-size: 11px; "
        "border: 1px solid #ccc;"
    );
    apiLayout->addWidget(apiTerminal);

    terminalLayout->addLayout(logLayout);
    terminalLayout->addLayout(apiLayout);

    mainLayout->addLayout(terminalLayout);

    // === Connexion des boutons ===
    connect(btnStart, &QPushButton::clicked, this, &aprsfi::onStartClicked);
    connect(btnForce, &QPushButton::clicked, this, &aprsfi::onForceClicked);
    connect(btnQuit, &QPushButton::clicked, this, &aprsfi::onQuitClicked);
}

void aprsfi::logToUI(const QString &message, bool isError) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString color = isError ? "#ff6b6b" : "#d4d4d4";
    QString icon = isError ? "" : "";

    QString formattedMessage = QString(
        "<span style='color: #888;'>[%1]</span> "
        "<span style='color: %2;'>%3 %4</span>"
    ).arg(timestamp, color, icon, message);

    logTerminal->append(formattedMessage);
}

void aprsfi::updateStatusLabel() {
    if (isRunning) {
        statusLabel->setText("En cours d'exécution");
        statusLabel->setStyleSheet(
            "background-color: #4CAF50; "
            "color: white; "
            "padding: 8px; "
            "border-radius: 4px; "
            "font-weight: bold;"
        );
    } else {
        statusLabel->setText("Arrêté");
        statusLabel->setStyleSheet(
            "background-color: #f0f0f0; "
            "padding: 8px; "
            "border-radius: 4px; "
            "font-weight: bold;"
        );
    }
}

// ==========================================
// CONFIGURATION
// ==========================================

void aprsfi::loadSettings() {
    config.apiCallsign = settings->value("API/name", "OH7RDA").toString();
    config.apiWhat = settings->value("API/what", "loc").toString();
    config.apiKey = settings->value("API/apikey", "").toString();
    config.apiInterval = settings->value("API/interval", 60000).toInt();

    config.dbHost = settings->value("Database/host", "172.18.58.85").toString();
    config.dbUser = settings->value("Database/username", "root").toString();
    config.dbPassword = settings->value("Database/password", "toto").toString();
    config.dbName = settings->value("Database/database", "ballon2026").toString();

    config.wsPort = settings->value("WebSocket/port", 12345).toInt();

    logToUI("Configuration chargée depuis config.ini");
}

// ==========================================
// ACTIONS DES BOUTONS
// ==========================================

void aprsfi::onStartClicked() {
    if (isRunning) {
        logToUI("⚠ Le backend est déjà en cours d'exécution");
        return;
    }

    logToUI("🚀 Démarrage du backend...");

    // 1. Connexion à la base de données
    if (!database->connect(config.dbHost, config.dbUser,
                           config.dbPassword, config.dbName)) {
        logToUI("Impossible de démarrer sans connexion BDD", true);
        return;
    }

    // 2. Configuration et démarrage de l'API
    apiClient->configure(config.apiCallsign, config.apiWhat,
                        config.apiKey, config.apiInterval);
    apiClient->startPolling();

    // 3. Démarrage du serveur WebSocket
    if (!wsServer->start(config.wsPort)) {
        logToUI("Impossible de démarrer le WebSocket", true);
        return;
    }

    // Mise à jour de l'état
    isRunning = true;
    btnStart->setEnabled(false);
    btnForce->setEnabled(true);
    updateStatusLabel();

    logToUI("Backend démarré avec succès !");
}

void aprsfi::onForceClicked() {
    if (!isRunning) {
        logToUI("Le backend doit être démarré d'abord", true);
        return;
    }

    logToUI("Requête forcée par l'utilisateur");
    apiClient->fetchNow();
}

void aprsfi::onQuitClicked() {
    logToUI("Fermeture de l'application...");
    QApplication::quit();
}

// ==========================================
// GESTION DES DONNÉES DE L'API
// ==========================================

void aprsfi::onApiDataReceived(const QJsonArray &entries) {
    // Sauvegarde chaque entrée dans la BDD
    for (const QJsonValue &value : entries) {
        QJsonObject entry = value.toObject();
        database->saveEntry(entry);
    }

    // Récupération des positions actuelles et broadcast WebSocket
    QJsonArray currentPositions = database->getCurrentPositions();
    wsServer->broadcastPositions(currentPositions);
}

void aprsfi::onApiRawResponse(const QString &json) {
    // Affichage du JSON brut dans le terminal API
    apiTerminal->setPlainText(json);
}

// ==========================================
// AFFICHAGE DES LOGS
// ==========================================

void aprsfi::onLogMessage(const QString &message) {
    logToUI(message);
}

void aprsfi::onErrorMessage(const QString &error) {
    logToUI(error, true);
}
