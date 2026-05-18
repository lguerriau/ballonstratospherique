#include "websocketserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>

WebSocketServer::WebSocketServer(QObject *parent)
    : QObject(parent),
      server(nullptr),
      port(0) {
}

WebSocketServer::~WebSocketServer() {
    stop();
}

bool WebSocketServer::start(int port) {
    if (server && server->isListening()) {
        emit logMessage("⚠ Le serveur WebSocket est déjà démarré");
        return false;
    }

    this->port = port;
    server = new QWebSocketServer(
        QStringLiteral("APRS WebSocket Server"),
        QWebSocketServer::NonSecureMode,
        this
    );

    if (!server->listen(QHostAddress::Any, port)) {
        emit errorOccurred("Impossible de démarrer le WebSocket : " + server->errorString());
        delete server;
        server = nullptr;
        return false;
    }

    connect(server, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);

    emit logMessage("✓ Serveur WebSocket démarré sur le port " + QString::number(port));
    return true;
}

void WebSocketServer::stop() {
    if (!server) return;

    // Déconnexion de tous les clients
    for (QWebSocket *client : std::as_const(clients)) {
        client->close();
        client->deleteLater();
    }
    clients.clear();

    // Fermeture du serveur
    server->close();
    server->deleteLater();
    server = nullptr;

    emit logMessage("Serveur WebSocket arrêté");
}

bool WebSocketServer::isRunning() const {
    return server && server->isListening();
}

int WebSocketServer::clientCount() const {
    return clients.size();
}

void WebSocketServer::broadcastPositions(const QJsonArray &positions) {
    if (clients.isEmpty()) {
        return; // Pas de clients connectés
    }

    // Pour chaque position dans le tableau
    for (const QJsonValue &value : positions) {
        QJsonObject pos = value.toObject();

        // Création du message JSON à envoyer
        QJsonObject message;
        message["type"] = "position_update";
        message["name"] = pos["name"].toString();
        message["lat"] = pos["lat"].toDouble();
        message["lng"] = pos["lng"].toDouble();
        message["lasttime"] = pos["lasttime"].toString();

        QJsonDocument doc(message);
        QString jsonString = doc.toJson(QJsonDocument::Compact);

        // Envoi à tous les clients
        for (QWebSocket *client : std::as_const(clients)) {
            client->sendTextMessage(jsonString);
        }
    }

    emit logMessage("→ Broadcast de " + QString::number(positions.size()) +
                    " position(s) à " + QString::number(clients.size()) + " client(s)");
}

void WebSocketServer::onNewConnection() {
    QWebSocket *socket = server->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived,
            this, &WebSocketServer::onTextMessageReceived);
    connect(socket, &QWebSocket::disconnected,
            this, &WebSocketServer::onSocketDisconnected);

    clients.append(socket);

    emit logMessage("✓ Client connecté (total: " + QString::number(clients.size()) + ")");
    emit clientConnected();
}

void WebSocketServer::onTextMessageReceived(const QString &message) {
    emit logMessage("← Message reçu d'un client : " + message);
    emit messageReceived(message);
}

void WebSocketServer::onSocketDisconnected() {
    QWebSocket *socket = qobject_cast<QWebSocket*>(sender());
    if (!socket) return;

    clients.removeAll(socket);
    socket->deleteLater();

    emit logMessage("✗ Client déconnecté (total: " + QString::number(clients.size()) + ")");
    emit clientDisconnected();
}
