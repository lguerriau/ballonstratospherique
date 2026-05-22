/**
 * @file apiclient.cpp
 * @brief Implémentation de la classe ApiClient
 * @details Gestion des requêtes HTTP vers l'API APRS.fi avec polling automatique
 * @version 4.0
 * @date 22/05/2026
 * @author Guerriau Lucien
 */

#include "apiclient.h"
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

/**
 * @brief Constructeur — initialise le gestionnaire réseau et le timer de polling
 * @param parent Objet parent Qt
 */
ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , networkManager(nullptr)
    , pollTimer(nullptr)
    , intervalMs(60000) {

    networkManager = new QNetworkAccessManager(this);
    pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &ApiClient::sendRequest);
}

/**
 * @brief Destructeur — arrête le polling si actif
 */
ApiClient::~ApiClient() {
    stopPolling();
}

/**
 * @brief Configure les paramètres de la requête API
 * @param callsign Indicatif APRS à interroger
 * @param what Type de données demandées ("loc" ou "wx")
 * @param apiKey Clé d'accès à l'API APRS.fi
 * @param intervalMs Intervalle de polling en millisecondes
 */
void ApiClient::configure(const QString &callsign, const QString &what, const QString &apiKey, int intervalMs) {
    this->callsign = callsign;
    this->what = what;
    this->apiKey = apiKey;
    this->intervalMs = intervalMs;
}

/**
 * @brief Démarre le polling périodique
 * @details Envoie immédiatement une première requête puis répète selon intervalMs.
 *          Sans effet si le polling est déjà actif.
 */
void ApiClient::startPolling() {
    if (!pollTimer->isActive()) {
        sendRequest();
        pollTimer->start(intervalMs);
        emit logMessage(QString("Boucle API demarree (%1) : %2 ms.").arg(what).arg(intervalMs));
    }
}

/**
 * @brief Arrête le polling périodique
 */
void ApiClient::stopPolling() {
    if (pollTimer->isActive()) {
        pollTimer->stop();
    }
}

/**
 * @brief Envoie une requête HTTP GET immédiate vers l'API APRS.fi
 * @details Construit l'URL avec les paramètres name, what, apikey et format=json
 */
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

/**
 * @brief Slot interne appelé à la fin d'une requête HTTP
 * @details Parse la réponse JSON, émet rawResponse puis dataReceived si résultat "ok",
 *          ou errorOccurred en cas d'erreur réseau ou API.
 */
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
