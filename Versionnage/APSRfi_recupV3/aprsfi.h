#ifndef APRSFI_H
#define APRSFI_H

#include <QWidget>
#include <QSettings>
#include "databasemanager.h"
#include "apiclient.h"
#include "websocketserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class aprsfi; }
QT_END_NAMESPACE

class aprsfi : public QWidget {
    Q_OBJECT

public:
    explicit aprsfi(QWidget *parent = nullptr);
    ~aprsfi();

private slots:
    // Slots connectés automatiquement par nom (on_NomDuBouton_clicked)
    void on_LancerServeur_clicked();
    void on_ForcerAPI_clicked();
    void on_Quitter_clicked();

    // Gestion des données et logs
    void onApiDataReceived(const QJsonArray &entries);
    void onApiRawResponse(const QString &json);
    void onLogMessage(const QString &message);
    void onErrorMessage(const QString &error);

private:
    Ui::aprsfi *ui;

    // Modules
    DatabaseManager *database;
    ApiClient *apiClient;
    WebSocketServer *wsServer;

    bool isRunning = false;

    // Structure de config simplifiée
    struct {
        QString callsign, what, apiKey, dbHost, dbUser, dbPass, dbName;
        int interval, wsPort;
    } config;

    void loadSettings();
    void logToUI(const QString &message, bool isError = false);
    void updateButtons();
};

#endif
