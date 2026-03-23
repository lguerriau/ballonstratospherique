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

// Si tu utilises le Designer Qt (fichier .ui)
QT_BEGIN_NAMESPACE
namespace Ui { class aprsfi; }
QT_END_NAMESPACE

// Ta classe hérite maintenant de QWidget
class aprsfi : public QWidget {
    Q_OBJECT

public:
    explicit aprsfi(QWidget *parent = nullptr);
    ~aprsfi();

    // On garde notre fonction de démarrage
    void startBackend();

private slots:
    void fetchApiData();
    void onApiReply(QNetworkReply *reply);
    void onNewWebSocketConnection();
    void processWebSocketMessage(QString message);
    void socketDisconnected();

private:
    Ui::aprsfi *ui; // Pointeur vers ton interface graphique

    // --- Les variables de notre backend ---
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
