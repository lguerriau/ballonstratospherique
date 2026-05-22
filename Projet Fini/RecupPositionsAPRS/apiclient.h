/**
 * @file apiclient.h
 * @brief Déclaration de la classe ApiClient
 * @details Gestion des requêtes HTTP vers l'API APRS.fi avec polling automatique
 * @version 4.0
 * @date 22/05/2026
 * @author Guerriau Lucien
 */

#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QJsonArray>

/**
 * @class ApiClient
 * @brief Client HTTP pour interroger l'API APRS.fi
 * @details Envoie des requêtes GET périodiques à l'API APRS.fi et émet les données
 *          reçues via des signaux Qt. Supporte deux types de données : positions (loc)
 *          et télémétries météo (wx).
 */
class ApiClient : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructeur de ApiClient
     * @param parent Objet parent Qt
     */
    explicit ApiClient(QObject *parent = nullptr);

    /**
     * @brief Destructeur — arrête le polling si actif
     */
    ~ApiClient();

    /**
     * @brief Configure les paramètres de la requête API
     * @param callsign Indicatif APRS à interroger
     * @param what Type de données demandées (ex: "loc" ou "wx")
     * @param apiKey Clé d'accès à l'API APRS.fi
     * @param intervalMs Intervalle de polling en millisecondes
     */
    void configure(const QString &callsign, const QString &what, const QString &apiKey, int intervalMs);

public slots:
    /**
     * @brief Démarre le polling périodique
     * @details Envoie immédiatement une première requête puis répète selon intervalMs
     */
    void startPolling();

    /**
     * @brief Arrête le polling périodique
     */
    void stopPolling();

    /**
     * @brief Envoie une requête HTTP GET immédiate vers l'API APRS.fi
     */
    void sendRequest();

signals:
    /**
     * @brief Émis lorsque des données valides sont reçues depuis l'API
     * @param entries Tableau JSON des entrées retournées
     * @param type Type de données correspondant au paramètre "what" configuré
     */
    void dataReceived(const QJsonArray &entries, const QString &type);

    /**
     * @brief Émis avec la réponse brute JSON de l'API
     * @param json Réponse JSON formatée
     * @param type Type de données correspondant au paramètre "what" configuré
     */
    void rawResponse(const QString &json, const QString &type);

    /**
     * @brief Émis pour journaliser un événement informationnel
     * @param message Texte du message de log
     */
    void logMessage(const QString &message);

    /**
     * @brief Émis en cas d'erreur réseau ou API
     * @param error Description de l'erreur
     */
    void errorOccurred(const QString &error);

private slots:
    /**
     * @brief Slot interne appelé à la fin d'une requête HTTP
     * @details Parse la réponse JSON et émet dataReceived ou errorOccurred
     */
    void onReplyFinished();

private:
    /** @brief Gestionnaire de requêtes réseau Qt */
    QNetworkAccessManager *networkManager;

    /** @brief Timer pour le polling périodique */
    QTimer *pollTimer;

    /** @brief Indicatif APRS interrogé */
    QString callsign;

    /** @brief Type de données demandées ("loc" ou "wx") */
    QString what;

    /** @brief Clé d'accès à l'API APRS.fi */
    QString apiKey;

    /** @brief Intervalle de polling en millisecondes */
    int intervalMs;
};

#endif // APICLIENT_H
