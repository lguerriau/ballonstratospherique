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

    // Une seule méthode de traitement, on utilisera le paramètre "type" pour différencier Loc et Wx
    void onApiDataReceived(const QJsonArray &entries, const QString &type);
    void onApiRawResponse(const QString &json, const QString &type);
    void onLogMessage(const QString &message);
    void onErrorMessage(const QString &error);

private:
    Ui::aprsfi *ui;

    DatabaseManager *database;
    ApiClient *apiClient;
    ApiClient *apiClientWx;
    WebSocketServer *wsServer;

    bool isRunning;

    QSettings *settings;
    QString apiName;
    QString apiWhat;
    QString apiWhatWx;
    QString apiKey;
    QString apiFormat;
    int apiInterval;

    void loadSettings();
    void logToUI(const QString &message, bool isError = false);
    void updateButtons();
};

#endif // APRSFI_H
