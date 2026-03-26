#ifndef APRSFI_H
#define APRSFI_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSqlDatabase>
#include <QTimer>
#include <QSettings>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class aprsfi; }
QT_END_NAMESPACE

class aprsfi : public QWidget {
    Q_OBJECT

public:
    explicit aprsfi(QWidget *parent = nullptr);
    ~aprsfi();

    // Fonction pour lancer toute la logique (à appeler depuis ton main.cpp)
    void startBackend();

private slots:
    void fetchApiData();
    void onApiReply(QNetworkReply *reply);
    void onNewWebSocketConnection();
    void processWebSocketMessage(QString message);
    void socketDisconnected();

private:
    Ui::aprsfi *ui;

    // --- Les variables et méthodes de notre backend ---
    void loadSettings();
    bool connectToDatabase();
    void insertIntoDatabase(const QJsonObject &entry);
    void broadcastUpdate();

    QSettings *settings;
    QNetworkAccessManager *networkManager;
    QSqlDatabase db;
    QTimer *pollTimer;

    QWebSocketServer *wsServer;
    QList<QWebSocket *> clients;

    QString apiName, apiWhat, apiKey, apiFormat;
    int apiInterval;
    QString dbHost, dbUser, dbPass, dbName;
    int wsPort;
};

#endif // APRSFI_H
