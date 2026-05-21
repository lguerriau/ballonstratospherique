#include "apiclient.h"
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , networkManager(nullptr)
    , pollTimer(nullptr)
    , intervalMs(60000) {

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
    if (!pollTimer->isActive()) {
        sendRequest();
        pollTimer->start(intervalMs);
        emit logMessage(QString("Boucle API demarree (%1) : %2 ms.").arg(what).arg(intervalMs));
    }
}

void ApiClient::stopPolling() {
    if (pollTimer->isActive()) {
        pollTimer->stop();
    }
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

    if (reply == nullptr) {
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(QString("Erreur API (%1) : %2").arg(what, reply->errorString()));
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    // On envoie le type de requête avec la raw response
    emit rawResponse(doc.toJson(QJsonDocument::Indented), what);

    QJsonObject obj = doc.object();
    QString result = obj["result"].toString();

    if (result == "ok") {
        QJsonArray entries = obj["entries"].toArray();
        int entryCount = entries.size();

        if (entryCount == 0) {
            emit logMessage(QString("API OK (%1) : Aucune donnee trouvee.").arg(what));
        } else {
            emit logMessage(QString("API OK (%1) : %2 donnee(s) recuperee(s).").arg(what).arg(entryCount));
            emit dataReceived(entries, what);
        }
    } else {
        emit errorOccurred("Erreur APRS.fi : " + obj["description"].toString());
    }

    reply->deleteLater();
}
