#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QJsonArray>

/**
 * ApiClient - Gère les requêtes HTTP vers l'API APRS.fi
 *
 * Responsabilités :
 * - Configuration de l'API (clé, paramètres)
 * - Envoi des requêtes périodiques
 * - Traitement des réponses JSON
 * - Gestion des erreurs réseau
 */
class ApiClient : public QObject {
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient();

    // Configuration de l'API
    void configure(const QString &callsign, const QString &what,
                   const QString &apiKey, int intervalMs);

    // Démarre les requêtes automatiques
    void startPolling();

    // Arrête les requêtes automatiques
    void stopPolling();

    // Force une requête immédiate
    void fetchNow();

    // Est-ce que le polling est actif ?
    bool isPolling() const;

signals:
    void dataReceived(const QJsonArray &entries);  // Données reçues de l'API
    void rawResponse(const QString &json);         // JSON brut pour affichage
    void logMessage(const QString &message);       // Pour envoyer des logs à l'UI
    void errorOccurred(const QString &error);      // Pour signaler les erreurs

private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager *networkManager;
    QTimer *pollTimer;

    // Paramètres de l'API
    QString callsign;
    QString what;
    QString apiKey;
    int intervalMs;

    // Envoi d'une requête
    void sendRequest();
};

#endif // APICLIENT_H
