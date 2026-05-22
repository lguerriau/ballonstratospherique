/**
 * @file websocketserver.cpp
 * @brief Implémentation de la classe WebSocketServer
 * @details Serveur WebSocket pour diffuser les positions APRS aux clients web
 * @version 4.0
 * @date 22/05/2026
 * @author Guerriau Lucien
 */

#include "websocketserver.h"
#include <QJsonDocument>
#include <QJsonObject>

/**
 * @brief Constructeur de WebSocketServer
 * @param parent Objet parent Qt
 */
WebSocketServer::WebSocketServer(QObject *parent)
    : QObject(parent)
    , server(nullptr)
    , m_port(0) {}

/**
 * @brief Destructeur — appelle stop() pour libérer les ressources
 */
WebSocketServer::~WebSocketServer() {
    stop();
}

/**
 * @brief Charge le port d'écoute depuis config.ini
 * @param settings Pointeur vers l'objet QSettings du fichier config.ini
 */
void WebSocketServer::loadConfig(QSettings *settings) {
    m_port = settings->value("WebSocket/port").toInt();
}

/**
 * @brief Démarre le serveur WebSocket en écoute sur m_port
 * @details Crée une instance QWebSocketServer en mode non sécurisé et
 *          connecte le signal newConnection.
 * @return WS_SUCCESS si le démarrage réussit, WsStatus d'erreur sinon
 */
WsStatus WebSocketServer::start() {
    WsStatus status = WS_ERROR_START_FAILED;

    if (server != nullptr && server->isListening()) {
        status = WS_ERROR_ALREADY_RUNNING;
    } else {
        server = new QWebSocketServer("APRS WebSocket Server", QWebSocketServer::NonSecureMode, this);

        if (server->listen(QHostAddress::Any, m_port)) {
            connect(server, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
            emit logMessage("Serveur WebSocket en ecoute sur le port " + QString::number(m_port));
            status = WS_SUCCESS;
        } else {
            emit errorOccurred("Impossible de demarrer le serveur WebSocket.");
            status = WS_ERROR_START_FAILED;
        }
    }
    return status;
}

/**
 * @brief Arrête le serveur et déconnecte tous les clients
 * @details Ferme chaque socket client, vide la liste, puis détruit le serveur.
 *          Sans effet si le serveur n'est pas démarré.
 */
void WebSocketServer::stop() {
    if (server == nullptr) return;

    for (QWebSocket *client : qAsConst(clients)) {
        client->close();
        client->deleteLater();
    }
    clients.clear();

    server->close();
    server->deleteLater();
    server = nullptr;
}

/**
 * @brief Diffuse les positions courantes à tous les clients connectés
 * @details Construit un message JSON par position avec les champs :
 *          type="position_update", name, lat, lng, temp, pressure, humidity.
 *          Sans effet si aucun client n'est connecté ou si le tableau est vide.
 * @param positions Tableau JSON des positions issues de DatabaseManager::getCurrentPositions()
 */
void WebSocketServer::broadcastPositions(const QJsonArray &positions) {
    if (!clients.isEmpty() && !positions.isEmpty()) {
        for (const QJsonValue &value : positions) {
            QJsonObject entry = value.toObject();

            QJsonObject msg;
            msg["type"] = "position_update";
            msg["name"] = entry.value("name").toString();
            msg["lat"] = entry.value("lat").toDouble();
            msg["lng"] = entry.value("lng").toDouble();
            msg["temp"] = entry.value("temp").toString();
            msg["pressure"] = entry.value("pressure").toString();
            msg["humidity"] = entry.value("humidity").toString();

            QString json = QJsonDocument(msg).toJson(QJsonDocument::Compact);

            for (QWebSocket *client : std::as_const(clients)) {
                if (client != nullptr) {
                    client->sendTextMessage(json);
                }
            }
            emit logMessage("Web update >> " + msg["name"].toString());
        }
    }
}

/**
 * @brief Slot appelé lors d'une nouvelle connexion cliente
 * @details Récupère le socket en attente, connecte ses signaux et l'ajoute à la liste
 */
void WebSocketServer::onNewConnection() {
    QWebSocket *socket = server->nextPendingConnection();

    if (socket != nullptr) {
        connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketServer::onTextMessageReceived);
        connect(socket, &QWebSocket::disconnected, this, &WebSocketServer::onSocketDisconnected);

        clients.append(socket);
        emit logMessage(QString("Nouveau client Web connecte. Total : %1").arg(clients.size()));
    }
}

/**
 * @brief Slot appelé à la réception d'un message texte d'un client
 * @param message Contenu du message reçu
 */
void WebSocketServer::onTextMessageReceived(const QString &message) {
    emit logMessage("Message recu du Web : " + message);
}

/**
 * @brief Slot appelé lors de la déconnexion d'un client
 * @details Retire le socket de la liste clients et le programme pour suppression
 */
void WebSocketServer::onSocketDisconnected() {
    QWebSocket *socket = qobject_cast<QWebSocket*>(sender());

    if (socket != nullptr) {
        clients.removeAll(socket);
        socket->deleteLater();
        emit logMessage(QString("Client Web deconnecte. Total : %1").arg(clients.size()));
    }
}
