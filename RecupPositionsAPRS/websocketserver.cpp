#include "websocketserver.h"
#include <QJsonDocument>
#include <QJsonObject>

WebSocketServer::WebSocketServer(QObject *parent)
    : QObject(parent)
    , server(nullptr)
    , port(0) {}

WebSocketServer::~WebSocketServer() {
    stop();
}

WsStatus WebSocketServer::start(int port) {
    WsStatus status = WS_ERROR_START_FAILED;

    // Power of 10: pas de early return
    if (server != nullptr && server->isListening()) {
        status = WS_ERROR_ALREADY_RUNNING;
    } else {
        this->port = port;
        server = new QWebSocketServer("APRS WebSocket Server", QWebSocketServer::NonSecureMode, this);

        if (server->listen(QHostAddress::Any, port)) {
            connect(server, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
            emit logMessage("Serveur WebSocket en ecoute sur le port " + QString::number(port));
            status = WS_SUCCESS;
        } else {
            emit errorOccurred("Impossible de demarrer le serveur WebSocket.");
            status = WS_ERROR_START_FAILED;
        }
    }

    return status;
}

void WebSocketServer::stop() {
    if (server != nullptr) {
        // Fermeture de tous les clients
        for (QWebSocket *client : std::as_const(clients)) {
            client->close();
            client->deleteLater();
        }
        clients.clear();

        server->close();
        server->deleteLater();
        server = nullptr;
    }
}

void WebSocketServer::broadcastPositions(const QJsonArray &positions) {
    // Power of 10 : vérifications explicites, flux linéaire continu
    if (!clients.isEmpty() && !positions.isEmpty()) {
        for (const QJsonValue &value : positions) {
            QJsonObject entry = value.toObject();

            QJsonObject msg;
            msg["type"] = "position_update";
            msg["name"] = entry.value("name").toString();
            msg["lat"] = entry.value("lat").toDouble();
            msg["lng"] = entry.value("lng").toDouble();

            // Transfert des variables de télémétrie lues depuis la BDD
            msg["temp"] = entry.value("temp").toString();
            msg["pressure"] = entry.value("pressure").toString();
            msg["humidity"] = entry.value("humidity").toString();
            msg["wind_direction"] = entry.value("wind_direction").toString();
            msg["wind_speed"] = entry.value("wind_speed").toString();

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

void WebSocketServer::onNewConnection() {
    QWebSocket *socket = server->nextPendingConnection();

    if (socket != nullptr) {
        connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketServer::onTextMessageReceived);
        connect(socket, &QWebSocket::disconnected, this, &WebSocketServer::onSocketDisconnected);

        clients.append(socket);
        emit logMessage(QString("Nouveau client Web connecte. Total : %1").arg(clients.size()));
    }
}

void WebSocketServer::onTextMessageReceived(const QString &message) {
    emit logMessage("Message recu du Web : " + message);
}

void WebSocketServer::onSocketDisconnected() {
    QWebSocket *socket = qobject_cast<QWebSocket*>(sender());

    if (socket != nullptr) {
        clients.removeAll(socket);
        socket->deleteLater();
        emit logMessage(QString("Client Web deconnecte. Total : %1").arg(clients.size()));
    }
}
