#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QList>
#include <QJsonArray>

class WebSocketServer : public QObject {
    Q_OBJECT

public:
    explicit WebSocketServer(QObject *parent = nullptr);
    ~WebSocketServer();

    bool start(int port);
    void stop();
    void broadcastPositions(const QJsonArray &positions);

signals:
    void logMessage(const QString &message);
    void errorOccurred(const QString &error);

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
