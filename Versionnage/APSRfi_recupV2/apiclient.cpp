#include "apiclient.h"
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent),
      intervalMs(60000) {  // Par défaut : 1 minute

    networkManager = new QNetworkAccessManager(this);
    pollTimer = new QTimer(this);

    connect(pollTimer, &QTimer::timeout, this, &ApiClient::sendRequest);
}

ApiClient::~ApiClient() {
    stopPolling();
}

void ApiClient::configure(const QString &callsign, const QString &what,
                          const QString &apiKey, int intervalMs) {
    this->callsign = callsign;
    this->what = what;
    this->apiKey = apiKey;
    this->intervalMs = intervalMs;

    emit logMessage("API configurée : " + callsign + " (interval: " +
                    QString::number(intervalMs/1000) + "s)");
}

void ApiClient::startPolling() {
    if (pollTimer->isActive()) {
        emit logMessage("⚠ Le polling est déjà actif");
        return;
    }

    // Première requête immédiate
    fetchNow();

    // Puis requêtes périodiques
    pollTimer->start(intervalMs);
    emit logMessage("✓ Polling démarré (toutes les " + QString::number(intervalMs/1000) + "s)");
}

void ApiClient::stopPolling() {
    if (pollTimer->isActive()) {
        pollTimer->stop();
        emit logMessage("Polling arrêté");
    }
}

void ApiClient::fetchNow() {
    emit logMessage("→ Envoi requête API...");
    sendRequest();
}

bool ApiClient::isPolling() const {
    return pollTimer->isActive();
}

void ApiClient::sendRequest() {
    // Construction de l'URL avec les paramètres
    QUrl url("https://api.aprs.fi/api/get");
    QUrlQuery query;
    query.addQueryItem("name", callsign);
    query.addQueryItem("what", what);
    query.addQueryItem("apikey", apiKey);
    query.addQueryItem("format", "json");
    url.setQuery(query);

    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    // Connexion du signal finished
    connect(reply, &QNetworkReply::finished, this, &ApiClient::onReplyFinished);
}

void ApiClient::onReplyFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    // Vérification des erreurs réseau
    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred("Erreur réseau : " + reply->errorString());
        emit rawResponse("Erreur : " + reply->errorString());
        reply->deleteLater();
        return;
    }

    // Lecture de la réponse
    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

    // Envoi du JSON brut pour affichage dans l'UI
    if (jsonDoc.isNull()) {
        emit rawResponse(QString::fromUtf8(response));
    } else {
        emit rawResponse(jsonDoc.toJson(QJsonDocument::Indented));
    }

    // Traitement du JSON
    if (!jsonDoc.isObject()) {
        emit errorOccurred("Réponse invalide : pas un objet JSON");
        reply->deleteLater();
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    QString result = jsonObj.value("result").toString();

    if (result != "ok") {
        QString description = jsonObj.value("description").toString();
        emit errorOccurred("Erreur API : " + description);
        reply->deleteLater();
        return;
    }

    // Extraction des entrées
    QJsonArray entries = jsonObj.value("entries").toArray();

    if (entries.isEmpty()) {
        emit logMessage("⚠ API OK mais aucune position trouvée");
    } else {
        emit logMessage("✓ " + QString::number(entries.size()) + " position(s) reçue(s)");
        emit dataReceived(entries);
    }

    reply->deleteLater();
}
