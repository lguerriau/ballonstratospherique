#include "apiclient.h"
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ApiClient::ApiClient(QObject *parent) : QObject(parent), intervalMs(60000) {
    networkManager = new QNetworkAccessManager(this);
    pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &ApiClient::sendRequest);
}

ApiClient::~ApiClient() {
    stopPolling();
}

void ApiClient::configure(const QString &callsign, const QString &what, const QString &apiKey, int intervalMs) {
    this->callsign = callsign;
    this->what = what;
    this->apiKey = apiKey;
    this->intervalMs = intervalMs;
}

void ApiClient::startPolling() {
    if (pollTimer->isActive()) return;
    fetchNow();
    pollTimer->start(intervalMs);
    emit logMessage(QString("Boucle API demarree (%1 ms).").arg(intervalMs));
}

void ApiClient::stopPolling() {
    if (pollTimer->isActive()) pollTimer->stop();
}

void ApiClient::fetchNow() {
    sendRequest();
}

void ApiClient::sendRequest() {
    QUrl url("https://api.aprs.fi/api/get");
    QUrlQuery query;
    query.addQueryItem("name", callsign);
    query.addQueryItem("what", what);
    query.addQueryItem("apikey", apiKey);
    query.addQueryItem("format", "json");
    url.setQuery(query);

    QNetworkReply *reply = networkManager->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &ApiClient::onReplyFinished);
}

void ApiClient::onReplyFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred("Erreur API : " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    emit rawResponse(doc.toJson(QJsonDocument::Indented));

    QJsonObject obj = doc.object();
    if (obj["result"].toString() == "ok") {
        QJsonArray entries = obj["entries"].toArray();
        if (entries.isEmpty()) {
            emit logMessage("API OK : Aucune position trouvee.");
        } else {
            emit logMessage(QString("API OK : %1 position(s) recuperee(s).").arg(entries.size()));
            emit dataReceived(entries);
        }
    } else {
        emit errorOccurred("Erreur APRS.fi : " + obj["description"].toString());
    }
    reply->deleteLater();
}
