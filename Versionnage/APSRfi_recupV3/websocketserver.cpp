#include "websocketserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>

WebSocketServer::WebSocketServer(QObject *parent)
    : QObject(parent), server(nullptr), port(0) {}

WebSocketServer::~WebSocketServer() {
    stop();
}

bool WebSocketServer::start(int port) {
    if (server && server->isListening()) return false;

    this->port = port;
    server = new QWebSocketServer("APRS WebSocket Server", QWebSocketServer::NonSecureMode, this);

    if (!server->listen(QHostAddress::Any, port)) {
        emit errorOccurred("Impossible de demarrer le serveur WebSocket.");
        return false;
    }

    connect(server, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
    emit logMessage("Serveur WebSocket en ecoute sur le port " + QString::number(port) + ".");
    return true;
}

void WebSocketServer::stop() {
    if (!server) return;

    for (QWebSocket *client : std::as_const(clients)) {
        client->close();
        client->deleteLater();
    }
    clients.clear();

    server->close();
    server->deleteLater();
    server = nullptr;

    emit logMessage("Serveur WebSocket arrete.");
}

bool WebSocketServer::isRunning() const {
    return server && server->isListening();
}

int WebSocketServer::clientCount() const {
    return clients.size();
}

void WebSocketServer::broadcastPositions(const QJsonArray &positions) {
    if (clients.isEmpty() || positions.isEmpty()) return;

    for (const QJsonValue &value : positions) {
        QJsonObject msg = value.toObject();
        msg["type"] = "position_update";
        QString json = QJsonDocument(msg).toJson(QJsonDocument::Compact);

        for (QWebSocket *client : std::as_const(clients)) {
            client->sendTextMessage(json);
        }
    }
    emit logMessage(QString("Mise a jour envoyee a %1 client(s).").arg(clients.size()));
}

void WebSocketServer::onNewConnection() {
    QWebSocket *socket = server->nextPendingConnection();
    connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketServer::onTextMessageReceived);
    connect(socket, &QWebSocket::disconnected, this, &WebSocketServer::onSocketDisconnected);

    clients.append(socket);
    emit logMessage(QString("Un client s'est connecte. Total : %1").arg(clients.size()));
    emit clientConnected();
}

void WebSocketServer::onTextMessageReceived(const QString &message) {
    emit logMessage("Message recu du client : " + message);
    emit messageReceived(message);
}

void WebSocketServer::onSocketDisconnected() {
    QWebSocket *socket = qobject_cast<QWebSocket*>(sender());
    if (!socket) return;

    clients.removeAll(socket);
    socket->deleteLater();

    emit logMessage(QString("Un client s'est deconnecte. Total : %1").arg(clients.size()));
    emit clientDisconnected();
}
