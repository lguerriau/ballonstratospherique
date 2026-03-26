#include "aprsfi.h"
#include "ui_aprsfi.h"

// Bibliothèques nécessaires pour le JSON, la BDD et le réseau
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

// ==========================================
// --- INITIALISATION ---
// ==========================================

aprsfi::aprsfi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aprsfi)
{
    ui->setupUi(this);

    // Initialisation avec ton chemin absolu vers le fichier config.ini
    QString configPath = "/home/USERS/ELEVES/CIEL2024/lguerriau/ProjetQT/APRSfi_recup/config.ini";
    settings = new QSettings(configPath, QSettings::IniFormat, this);

    networkManager = new QNetworkAccessManager(this);
    pollTimer = new QTimer(this);

    connect(pollTimer, &QTimer::timeout, this, &aprsfi::fetchApiData);
}

aprsfi::~aprsfi() {
    if (wsServer) {
        wsServer->close();
    }
    qDeleteAll(clients.begin(), clients.end());
    delete ui;
}

// ==========================================
// --- DÉMARRAGE DU BACKEND ---
// ==========================================

void aprsfi::startBackend() {
    loadSettings();

    if (!connectToDatabase()) {
        qCritical() << "Impossible de démarrer sans BDD. Vérifiez config.ini et MariaDB.";
        return;
    }

    wsServer = new QWebSocketServer(QStringLiteral("APRS WebSocket Server"),
                                    QWebSocketServer::NonSecureMode, this);

    if (wsServer->listen(QHostAddress::Any, wsPort)) {
        qInfo() << "Serveur WebSocket démarré sur le port" << wsPort;
        connect(wsServer, &QWebSocketServer::newConnection,
                this, &aprsfi::onNewWebSocketConnection);
    } else {
        qCritical() << "Erreur de démarrage WebSocket:" << wsServer->errorString();
    }

    fetchApiData();
    pollTimer->start(apiInterval);
    qInfo() << "Boucle de requêtes API démarrée (Intervalle:" << apiInterval << "ms).";
}

void aprsfi::loadSettings() {
    apiName = settings->value("API/name", "OH7RDA").toString(); // Ex: F4KMN-1,F4KMN-2
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

    if (!db.open()) {
        qCritical() << "Erreur de connexion à MariaDB:" << db.lastError().text();
        return false;
    }
    qInfo() << "Connecté à MariaDB avec succès sur" << dbHost;
    return true;
}

// ==========================================
// --- GESTION DE L'API HTTP ---
// ==========================================

void aprsfi::fetchApiData() {
    QUrl url("https://api.aprs.fi/api/get");

    QUrlQuery query;
    query.addQueryItem("name", apiName);
    query.addQueryItem("what", apiWhat);
    query.addQueryItem("apikey", apiKey);
    query.addQueryItem("format", apiFormat);
    url.setQuery(query);

    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onApiReply(reply); });
}

void aprsfi::onApiReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Erreur API:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

    if (jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();

        if (jsonObj.value("result").toString() == "ok") {
            QJsonArray entries = jsonObj.value("entries").toArray();
            if (!entries.isEmpty()) {
                // On boucle pour traiter TOUS les indicatifs reçus (Multi-suivi)
                for (const QJsonValue &value : entries) {
                    QJsonObject entry = value.toObject();
                    insertIntoDatabase(entry);
                }

                // On avertit le site web une fois que tout est inséré
                broadcastUpdate();
            }
        }
    }
    reply->deleteLater();
}

// ==========================================
// --- GESTION DE LA BASE DE DONNÉES ---
// ==========================================

void aprsfi::insertIntoDatabase(const QJsonObject &entry) {
    // --- 1. Insertion dans la table HISTORIQUE ---
    QSqlQuery query(db);
    query.prepare("INSERT INTO HISTORIQUE (name, type, time, lasttime, lat, lng, symbol, srccall, dstcall, phg, comment, path) "
                  "VALUES (:name, :type, :time, :lasttime, :lat, :lng, :symbol, :srccall, :dstcall, :phg, :comment, :path)");

    query.bindValue(":name", entry.value("name").toString());
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
        qWarning() << "Erreur d'insertion dans HISTORIQUE:" << query.lastError().text();
    }

    // --- 2. Mise à jour de la table POSITION pour l'indicatif traité ---
    QSqlQuery posQuery(db);

    // On met à jour la ligne OÙ le nom correspond
    posQuery.prepare("UPDATE POSITION SET lat = :lat, lng = :lng, lasttime = :lasttime WHERE name = :name");
    posQuery.bindValue(":lat", entry.value("lat").toString());
    posQuery.bindValue(":lng", entry.value("lng").toString());
    posQuery.bindValue(":lasttime", entry.value("lasttime").toString());
    posQuery.bindValue(":name", entry.value("name").toString());

    if (!posQuery.exec()) {
        qWarning() << "Erreur de mise à jour dans POSITION:" << posQuery.lastError().text();
    } else {
        // Si aucune ligne n'a été mise à jour (l'indicatif n'existe pas encore dans la table)
        if (posQuery.numRowsAffected() == 0) {
            // On insère ce nouvel indicatif
            posQuery.prepare("INSERT INTO POSITION (name, lat, lng, lasttime) VALUES (:name, :lat, :lng, :lasttime)");
            posQuery.bindValue(":name", entry.value("name").toString());
            posQuery.bindValue(":lat", entry.value("lat").toString());
            posQuery.bindValue(":lng", entry.value("lng").toString());
            posQuery.bindValue(":lasttime", entry.value("lasttime").toString());

            if (!posQuery.exec()) {
                qWarning() << "Erreur d'insertion dans POSITION:" << posQuery.lastError().text();
            }
        }
        qInfo() << "BDD à jour pour" << entry.value("name").toString() << "| Lat:" << entry.value("lat").toString() << "Lng:" << entry.value("lng").toString();
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
    qInfo() << "Nouveau client web connecté ! Total clients:" << clients.size();
}

void aprsfi::processWebSocketMessage(QString message) {
    qDebug() << "Message reçu du web:" << message;
}

void aprsfi::socketDisconnected() {
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        clients.removeAll(pClient);
        pClient->deleteLater();
        qInfo() << "Client déconnecté. Total clients restants:" << clients.size();
    }
}

void aprsfi::broadcastUpdate() {
    QString updateMessage = "{\"status\": \"new_data_available\"}";
    for (QWebSocket *client : std::as_const(clients)) {
        client->sendTextMessage(updateMessage);
    }
}
