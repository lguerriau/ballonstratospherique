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

// Nouveaux includes pour l'interface graphique
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class aprsfi : public QWidget {
    Q_OBJECT

public:
    explicit aprsfi(QWidget *parent = nullptr);
    ~aprsfi();

private slots:
    void startBackend(); // Devient un slot pour le bouton "Lancer"
    void fetchApiData();
    void onApiReply(QNetworkReply *reply);
    void onNewWebSocketConnection();
    void processWebSocketMessage(QString message);
    void socketDisconnected();

private:
    // --- Elements de l'interface graphique ---
    QPushButton *btnStart;
    QPushButton *btnForce;
    QPushButton *btnQuit;
    QTextEdit *logTerminal;
    QTextEdit *apiTerminal;

    // --- Fonction utilitaire pour ecrire dans le terminal UI ---
    void logToUI(const QString &message);

    // --- Variables du backend ---
    void setupUI(); // Fonction pour construire l'interface
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
    bool isBackendRunning; // Pour eviter de lancer 2 fois
};

#endif // APRSFI_H
