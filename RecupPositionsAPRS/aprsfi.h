#ifndef APRSFI_H
#define APRSFI_H

#include <QWidget>
#include <QSettings>
#include <QString>
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
    void on_LancerServeur_clicked();
    void on_ForcerAPI_clicked();
    void on_Quitter_clicked();

    void onApiDataReceived(const QJsonArray &entries);
    void onApiRawResponse(const QString &json);
    void onLogMessage(const QString &message);
    void onErrorMessage(const QString &error);

private:
    Ui::aprsfi *ui;

    DatabaseManager *database;
    ApiClient *apiClient;
    WebSocketServer *wsServer;

    bool isRunning = false;

    // --- NOUVELLES VARIABLES DE CONFIGURATION ---
    QSettings *settings;
    QString apiName, apiWhat, apiKey, apiFormat;
    int apiInterval;
    QString dbHost, dbUser, dbPass, dbName;
    int wsPort;

    void loadSettings();
    // --------------------------------------------

    void logToUI(const QString &message, bool isError = false);
    void updateButtons();
};

#endif // APRSFI_H
