#ifndef APRSFI_H
#define APRSFI_H

#include <QWidget>
#include <QSettings>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>

// Les 3 modules principaux
#include "databasemanager.h"
#include "apiclient.h"
#include "websocketserver.h"

/**
 * aprsfi - Interface graphique principale
 *
 * Orchestre les 3 modules :
 * - DatabaseManager : gestion de la BDD
 * - ApiClient : requêtes HTTP vers APRS.fi
 * - WebSocketServer : diffusion temps réel aux clients web
 */
class aprsfi : public QWidget {
    Q_OBJECT

public:
    explicit aprsfi(QWidget *parent = nullptr);
    ~aprsfi();

private slots:
    // Actions des boutons
    void onStartClicked();
    void onForceClicked();
    void onQuitClicked();

    // Gestion des données de l'API
    void onApiDataReceived(const QJsonArray &entries);
    void onApiRawResponse(const QString &json);

    // Affichage des logs dans l'UI
    void onLogMessage(const QString &message);
    void onErrorMessage(const QString &error);

private:
    // === Interface graphique ===
    QPushButton *btnStart;
    QPushButton *btnForce;
    QPushButton *btnQuit;
    QTextEdit *logTerminal;
    QTextEdit *apiTerminal;
    QLabel *statusLabel;

    void setupUI();
    void logToUI(const QString &message, bool isError = false);
    void updateStatusLabel();

    // === Configuration ===
    QSettings *settings;
    void loadSettings();

    // === Les 3 modules principaux ===
    DatabaseManager *database;
    ApiClient *apiClient;
    WebSocketServer *wsServer;

    // === État du backend ===
    bool isRunning;

    // Paramètres chargés depuis config.ini
    struct Config {
        QString apiCallsign;
        QString apiWhat;
        QString apiKey;
        int apiInterval;
        QString dbHost;
        QString dbUser;
        QString dbPassword;
        QString dbName;
        int wsPort;
    } config;
};

#endif // APRSFI_H
