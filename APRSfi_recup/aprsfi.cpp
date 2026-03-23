#include "aprsfi.h"
#include "ui_aprsfi.h" // Fichier généré automatiquement par Qt Designer

// Les bibliothèques nécessaires pour le JSON, la BDD et le réseau
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

// ==========================================
// --- INITIALISATION DE L'INTERFACE ET DES OUTILS ---
// ==========================================

aprsfi::aprsfi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aprsfi) // Création de l'interface graphique
{
    ui->setupUi(this); // Charge les éléments visuels (même s'il n'y en a pas encore)

    // Initialisation des outils backend
    // On utilise maintenant ton chemin absolu pour être sûr de toujours trouver le fichier
    QString configPath = "/home/USERS/ELEVES/CIEL2024/lguerriau/ProjetQT/APRSfi_recup/config.ini";
    settings = new QSettings(configPath, QSettings::IniFormat, this);

    networkManager = new QNetworkAccessManager(this);
    pollTimer = new QTimer(this);

    // On relie le tic-tac du timer à notre fonction qui interroge l'API
    connect(pollTimer, &QTimer::timeout, this, &aprsfi::fetchApiData);
}

aprsfi::~aprsfi() {
    // Nettoyage propre quand on ferme l'application
    if (wsServer) {
        wsServer->close();
    }
    qDeleteAll(clients.begin(), clients.end()); // Déconnecte tous les clients web
    delete ui; // Détruit l'interface graphique
}

// ==========================================
// --- DÉMARRAGE DU BACKEND ---
// ==========================================

void aprsfi::startBackend() {
    loadSettings(); // 1. On lit le fichier .ini

    if (!connectToDatabase()) { // 2. On tente de se connecter à MariaDB
        qCritical() << "Impossible de démarrer sans BDD. Vérifiez config.ini et MariaDB.";
        // Si tu veux afficher une popup d'erreur sur l'UI plus tard, c'est ici !
        return;
    }

    // 3. Démarrage du serveur WebSocket (NonSecureMode = ws://, pas wss://)
    wsServer = new QWebSocketServer(QStringLiteral("APRS WebSocket Server"),
                                    QWebSocketServer::NonSecureMode, this);

    // Si le serveur arrive à écouter sur le port défini dans le .ini
    if (wsServer->listen(QHostAddress::Any, wsPort)) {
        qInfo() << "Serveur WebSocket démarré sur le port" << wsPort;
        // On connecte le signal d'une nouvelle connexion à notre fonction de gestion
        connect(wsServer, &QWebSocketServer::newConnection,
                this, &aprsfi::onNewWebSocketConnection);
    } else {
        qCritical() << "Erreur de démarrage WebSocket:" << wsServer->errorString();
    }

    // 4. On lance la première requête immédiatement, puis on démarre le timer
    fetchApiData();
    pollTimer->start(apiInterval); // apiInterval est en millisecondes
    qInfo() << "Boucle de requêtes API démarrée (Intervalle:" << apiInterval << "ms).";
}

void aprsfi::loadSettings() {
    // Lecture du config.ini. Le 2ème paramètre (ex: "OH7RDA") est la valeur par défaut
    apiName = settings->value("API/name", "OH7RDA").toString();
    apiWhat = settings->value("API/what", "loc").toString();
    apiKey = settings->value("API/apikey", "").toString();
    apiFormat = settings->value("API/format", "json").toString();
    apiInterval = settings->value("API/interval", 60000).toInt();

    dbHost = settings->value("Database/host", "172.18.58.85").toString();
    dbUser = settings->value("Database/username", "root").toString();
    dbPass = settings->value("Database/password", "toto").toString();
    dbName = settings->value("Database/database", "aprs").toString();

    wsPort = settings->value("WebSocket/port", 12345).toInt();
}

bool aprsfi::connectToDatabase() {
    // QMYSQL est le driver Qt pour MySQL et MariaDB
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

    // QUrlQuery est la façon propre de construire "?name=...&what=..."
    QUrlQuery query;
    query.addQueryItem("name", apiName);
    query.addQueryItem("what", apiWhat);
    query.addQueryItem("apikey", apiKey);
    query.addQueryItem("format", apiFormat);
    url.setQuery(query);

    // On prépare et on envoie la requête GET (asynchrone)
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    // Quand l'API a fini de répondre, on appelle onApiReply
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onApiReply(reply); });
}

void aprsfi::onApiReply(QNetworkReply *reply) {
    // Vérification des erreurs HTTP (ex: 404, 500, pas d'internet)
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Erreur API:" << reply->errorString();
        reply->deleteLater(); // On libère la mémoire
        return;
    }

    // On lit tout le texte renvoyé par l'API
    QByteArray response = reply->readAll();

    // On transforme ce texte brut en un objet JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

    if (jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();

        // On vérifie que l'API nous dit que tout s'est bien passé
        if (jsonObj.value("result").toString() == "ok") {
            // On extrait le tableau "entries" de la réponse JSON
            QJsonArray entries = jsonObj.value("entries").toArray();
            if (!entries.isEmpty()) {
                // On prend le premier élément (le point le plus récent)
                QJsonObject latestEntry = entries.first().toObject();

                // On envoie cet objet JSON à notre fonction BDD
                insertIntoDatabase(latestEntry);

                // L'insertion s'est bien passée, on avertit le site web !
                broadcastUpdate();
            }
        }
    }
    reply->deleteLater(); // Ne pas oublier de libérer la mémoire !
}

// ==========================================
// --- GESTION DE LA BASE DE DONNÉES ---
// ==========================================

void aprsfi::insertIntoDatabase(const QJsonObject &entry) {
    // --- 1. Insertion dans la table HISTORIQUE ---
    QSqlQuery query(db);
    // On utilise prepare() et des variables ":nom" pour éviter les injections SQL.
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

    // --- 2. Mise à jour de la table POSITION ---
    QSqlQuery posQuery(db);
    // On efface d'abord tout ce qu'il y a dans la table POSITION.
    posQuery.exec("DELETE FROM POSITION");

    // On insère le nouveau point
    posQuery.prepare("INSERT INTO POSITION (lat, lng, lasttime) VALUES (:lat, :lng, :lasttime)");
    posQuery.bindValue(":lat", entry.value("lat").toString());
    posQuery.bindValue(":lng", entry.value("lng").toString());
    posQuery.bindValue(":lasttime", entry.value("lasttime").toString());

    if (!posQuery.exec()) {
        qWarning() << "Erreur d'insertion dans POSITION:" << posQuery.lastError().text();
    } else {
        qInfo() << "BDD mise à jour. Lat:" << entry.value("lat").toString() << "Lng:" << entry.value("lng").toString();
    }
}

// ==========================================
// --- GESTION DU WEBSOCKET ---
// ==========================================

void aprsfi::onNewWebSocketConnection() {
    // Un navigateur web vient de se connecter !
    QWebSocket *pSocket = wsServer->nextPendingConnection();

    // On écoute ce client s'il nous parle ou s'il se déconnecte
    connect(pSocket, &QWebSocket::textMessageReceived, this, &aprsfi::processWebSocketMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &aprsfi::socketDisconnected);

    // On l'ajoute à notre liste de clients actifs
    clients << pSocket;
    qInfo() << "Nouveau client web connecté ! Total clients:" << clients.size();
}

void aprsfi::processWebSocketMessage(QString message) {
    // Utile si le site web a besoin d'envoyer des infos au serveur C++
    qDebug() << "Message reçu du web:" << message;
}

void aprsfi::socketDisconnected() {
    // Un client (navigateur) a fermé sa page web.
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        clients.removeAll(pClient); // On le retire de la liste
        pClient->deleteLater();     // On libère la mémoire
        qInfo() << "Client déconnecté. Total clients restants:" << clients.size();
    }
}

void aprsfi::broadcastUpdate() {
    // On crée un petit message JSON standardisé.
    QString updateMessage = "{\"status\": \"new_data_available\"}";

    // On parcourt la liste de TOUS les navigateurs connectés et on leur envoie le message.
    for (QWebSocket *client : std::as_const(clients)) {
        client->sendTextMessage(updateMessage);
    }
}
