#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QJsonArray>

class ApiClient : public QObject {
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient();

    void configure(const QString &callsign, const QString &what, const QString &apiKey, int intervalMs);
    void startPolling();
    void stopPolling();
    void sendRequest(); // Public pour permettre les appels manuels

signals:
    void dataReceived(const QJsonArray &entries);
    void rawResponse(const QString &json);
    void logMessage(const QString &message);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager *networkManager;
    QTimer *pollTimer;
    QString callsign;
    QString what;
    QString apiKey;
    int intervalMs;
};

#endif // APICLIENT_H
