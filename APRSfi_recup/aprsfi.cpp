#include "aprsfi.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>
#include <QDateTime>
#include <QApplication>

// ==========================================
// --- INITIALISATION ---
// ==========================================

aprsfi::aprsfi(QWidget *parent) :
    QWidget(parent),
    isBackendRunning(false)
{
    setupUI(); // Construit l'interface graphique

    // Chemin absolu vers le config.ini
    QString configPath = "/home/USERS/ELEVES/CIEL2024/lguerriau/ProjetQT/APRSfi_recup/config.ini";
    settings = new QSettings(configPath, QSettings::IniFormat, this);

    networkManager = new QNetworkAccessManager(this);
    pollTimer = new QTimer(this);

    connect(pollTimer, &QTimer::timeout, this, &aprsfi::fetchApiData);
}

aprsfi::~aprsfi() {
    if (wsServer) wsServer->close();
    qDeleteAll(clients.begin(), clients.end());
}

// ==========================================
// --- CREATION DE L'INTERFACE GRAPHIQUE ---
// ==========================================

void aprsfi::setupUI() {
    this->setWindowTitle("Dashboard APRS Backend");
    this->resize(900, 500); // Taille de base de la fenetre

    // Layout principal vertical
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- 1. Zone des boutons (en haut) ---
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnStart = new QPushButton("Lancer le Serveur", this);
    btnForce = new QPushButton("Forcer une requete API", this);
    btnQuit = new QPushButton("Quitter", this);

    btnForce->setEnabled(false); // Desactive tant que le serveur n'est pas lance

    buttonLayout->addWidget(btnStart);
    buttonLayout->addWidget(btnForce);
    buttonLayout->addWidget(btnQuit);

    // --- 2. Zone des terminaux (en bas) ---
    QHBoxLayout *terminalLayout = new QHBoxLayout();

    // Terminal Log (Gauche)
    QVBoxLayout *logLayout = new QVBoxLayout();
    logLayout->addWidget(new QLabel("Logs du Programme :", this));
    logTerminal = new QTextEdit(this);
    logTerminal->setReadOnly(true);
    logLayout->addWidget(logTerminal);

    // Terminal API (Droite)
    QVBoxLayout *apiLayout = new QVBoxLayout();
    apiLayout->addWidget(new QLabel("Reponse API (Brute) :", this));
    apiTerminal = new QTextEdit(this);
    apiTerminal->setReadOnly(true);
    apiTerminal->setStyleSheet("background-color: #f4f4f4; font-family: monospace;");
    apiLayout->addWidget(apiTerminal);

    terminalLayout->addLayout(logLayout);
    terminalLayout->addLayout(apiLayout);

    // --- Assemblage final ---
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(terminalLayout);

    // --- Connexion des boutons ---
    connect(btnStart, &QPushButton::clicked, this, &aprsfi::startBackend);
    connect(btnForce, &QPushButton::clicked, this, &aprsfi::fetchApiData);
    connect(btnQuit, &QPushButton::clicked, qApp, &QApplication::quit);

    logToUI("Interface initialisee. En attente du lancement...");
}

void aprsfi::logToUI(const QString &message) {
    QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss");
    logTerminal->append("[" + timeStr + "] " + message);
    qDebug() << message; // Garde aussi l'affichage dans la console classique
}

// ==========================================
// --- DEMARRAGE DU BACKEND ---
// ==========================================

void aprsfi::startBackend() {
    if (isBackendRunning) return; // Evite de lancer 2 fois

    logToUI("Demarrage du backend...");
    loadSettings();

    if (!connectToDatabase()) {
        logToUI("ERREUR : Impossible de se connecter a la BDD MariaDB.");
        return;
    }

    wsServer = new QWebSocketServer(QStringLiteral("APRS WebSocket Server"), QWebSocketServer::NonSecureMode, this);

    if (wsServer->listen(QHostAddress::Any, wsPort)) {
        logToUI("Serveur WebSocket demarre sur le port " + QString::number(wsPort));
        connect(wsServer, &QWebSocketServer::newConnection, this, &aprsfi::onNewWebSocketConnection);
    } else {
        logToUI("ERREUR WebSocket : " + wsServer->errorString());
    }

    isBackendRunning = true;
    btnStart->setEnabled(false); // Desactive le bouton Lancer
    btnForce->setEnabled(true);  // Active le bouton Forcer

    fetchApiData(); // Premiere requete immediate
    pollTimer->start(apiInterval);
    logToUI("Boucle API demarree. Intervalle : " + QString::number(apiInterval) + " ms.");
}

void aprsfi::loadSettings() {
    apiName = settings->value("API/name", "OH7RDA").toString();
    apiWhat = settings->value("API/what", "loc").toString();
    apiKey = settings->value("API/apikey", "").toString();
    apiFormat = settings->value("API/format", "json").toString();
    apiInterval = settings->value("API/interval", 60000).toInt();

    dbHost = settings->value("Database/host", "172.18.58.85").toString();
    dbUser = settings->value("Database/username", "root").toString();
    dbPass = settings->value("Database/password", "toto").toString();
    dbName = settings->value("Database/database", "ballon2026").toString();
    wsPort = settings->value("WebSocket/port", 12345).toInt();
}

bool aprsfi::connectToDatabase() {
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(dbHost);
    db.setUserName(dbUser);
    db.setPassword(dbPass);
    db.setDatabaseName(dbName);
    return db.open();
}

// ==========================================
// --- GESTION DE L'API HTTP ---
// ==========================================

void aprsfi::fetchApiData() {
    logToUI("Envoi de la requete API...");
    apiTerminal->clear(); // On vide l'ancien JSON
    apiTerminal->append("En attente de reponse...");

    QUrl url("https://api.aprs.fi/api/get");
    QUrlQuery query;
    query.addQueryItem("name", apiName);
    query.addQueryItem("what", apiWhat);
    query.addQueryItem("apikey", apiKey);
    query.addQueryItem("format", apiFormat);
    url.setQuery(query);
    logToUI("URL envoyee : " + url.toString());
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onApiReply(reply); });
}

void aprsfi::onApiReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        logToUI("ERREUR RESEAU : " + reply->errorString());
        apiTerminal->setPlainText("Erreur reseau : " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();

    // Affichage dans le terminal de droite (API)
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    if (jsonDoc.isNull()) {
        apiTerminal->setPlainText(QString::fromUtf8(response)); // Affiche en texte brut si pas JSON
    } else {
        // Affiche le JSON joliment indente
        apiTerminal->setPlainText(jsonDoc.toJson(QJsonDocument::Indented));
    }

    if (jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();
        if (jsonObj.value("result").toString() == "ok") {
            QJsonArray entries = jsonObj.value("entries").toArray();
            if (!entries.isEmpty()) {
                logToUI("Succes API. " + QString::number(entries.size()) + " position(s) recuperee(s).");
                for (const QJsonValue &value : entries) {
                    QJsonObject entry = value.toObject();
                    insertIntoDatabase(entry);
                }
                broadcastUpdate();
            } else {
                logToUI("ATTENTION : L'API a repondu OK, mais aucune position trouvee (tableau vide).");
            }
        } else {
            logToUI("ERREUR API : " + jsonObj.value("description").toString());
        }
    } else {
        logToUI("ERREUR : Impossible de decoder le JSON.");
    }

    reply->deleteLater();
}

// ==========================================
// --- GESTION DE LA BASE DE DONNEES ---
// ==========================================

void aprsfi::insertIntoDatabase(const QJsonObject &entry) {
    QString name = entry.value("name").toString();
    QSqlQuery query(db);

    query.prepare("INSERT INTO HISTORIQUE (name, type, time, lasttime, lat, lng, symbol, srccall, dstcall, phg, comment, path) "
                  "VALUES (:name, :type, :time, :lasttime, :lat, :lng, :symbol, :srccall, :dstcall, :phg, :comment, :path)");

    query.bindValue(":name", name);
    query.bindValue(":type", entry.value("type").toString());
    query.bindValue(":time", entry.value("time").toString());
    query.bindValue(":lasttime", entry.value("lasttime").toString());
    query.bindValue(":lat", entry.value("lat").toString());
    query.bindValue(":lng", entry.value("lng").toString());
    query.bindValue(":symbol", entry.value("symbol").toString());
    query.bindValue(":srccall", entry.value("srccall").toString());
    query.bindValue(":dstcall", entry.value("dstcall").toString());
    query.bindValue(":phg", entry.value("phg").toString());
    query.bindValue(":comment", entry.value("comment").toString());
    query.bindValue(":path", entry.value("path").toString());

    if (!query.exec()) {
        logToUI("Erreur SQL HISTORIQUE : " + query.lastError().text());
    }

    QSqlQuery posQuery(db);
    posQuery.prepare("UPDATE POSITION SET lat = :lat, lng = :lng, lasttime = :lasttime WHERE name = :name");
    posQuery.bindValue(":lat", entry.value("lat").toString());
    posQuery.bindValue(":lng", entry.value("lng").toString());
    posQuery.bindValue(":lasttime", entry.value("lasttime").toString());
    posQuery.bindValue(":name", name);

    if (!posQuery.exec()) {
        logToUI("Erreur SQL POSITION (Update) : " + posQuery.lastError().text());
    } else {
        if (posQuery.numRowsAffected() == 0) {
            posQuery.prepare("INSERT INTO POSITION (name, lat, lng, lasttime) VALUES (:name, :lat, :lng, :lasttime)");
            posQuery.bindValue(":name", name);
            posQuery.bindValue(":lat", entry.value("lat").toString());
            posQuery.bindValue(":lng", entry.value("lng").toString());
            posQuery.bindValue(":lasttime", entry.value("lasttime").toString());
            if (!posQuery.exec()) logToUI("Erreur SQL POSITION (Insert) : " + posQuery.lastError().text());
        }
        logToUI("BDD a jour pour " + name + " !");
    }
}

// ==========================================
// --- GESTION DU WEBSOCKET ---
// ==========================================

void aprsfi::onNewWebSocketConnection() {
    QWebSocket *pSocket = wsServer->nextPendingConnection();
    connect(pSocket, &QWebSocket::textMessageReceived, this, &aprsfi::processWebSocketMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &aprsfi::socketDisconnected);
    clients << pSocket;
    logToUI("Client Web connecte. Total : " + QString::number(clients.size()));
}

void aprsfi::processWebSocketMessage(QString message) {
    logToUI("Message recu du Web : " + message);
}

void aprsfi::socketDisconnected() {
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        clients.removeAll(pClient);
        pClient->deleteLater();
        logToUI("Client Web deconnecte. Total : " + QString::number(clients.size()));
    }
}

void aprsfi::broadcastUpdate() {
    QString updateMessage = "{\"status\": \"new_data_available\"}";
    for (QWebSocket *client : std::as_const(clients)) {
        client->sendTextMessage(updateMessage);
    }
}
