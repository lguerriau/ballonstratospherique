#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QList>
#include <QJsonArray>

/**
 * WebSocketServer - Gère le serveur WebSocket et les clients connectés
 *
 * Responsabilités :
 * - Démarrage du serveur WebSocket
 * - Gestion des connexions/déconnexions des clients
 * - Broadcast des mises à jour de positions
 * - Réception des messages des clients
 */
class WebSocketServer : public QObject {
    Q_OBJECT

public:
    explicit WebSocketServer(QObject *parent = nullptr);
    ~WebSocketServer();

    // Démarre le serveur sur un port donné
    bool start(int port);

    // Arrête le serveur
    void stop();

    // Est-ce que le serveur est actif ?
    bool isRunning() const;

    // Nombre de clients connectés
    int clientCount() const;

    // Envoie les positions à tous les clients
    void broadcastPositions(const QJsonArray &positions);

signals:
    void logMessage(const QString &message);       // Pour envoyer des logs à l'UI
    void errorOccurred(const QString &error);      // Pour signaler les erreurs
    void clientConnected();                        // Un client s'est connecté
    void clientDisconnected();                     // Un client s'est déconnecté
    void messageReceived(const QString &message);  // Message reçu d'un client

private slots:
    void onNewConnection();
    void onTextMessageReceived(const QString &message);
    void onSocketDisconnected();

private:
    QWebSocketServer *server;
    QList<QWebSocket*> clients;
    int port;
};

#endif // WEBSOCKETSERVER_H
