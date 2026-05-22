/**
 * @file aprsfi.h
 * @brief Déclaration de la classe aprsfi
 * @details Fenêtre principale de l'application — orchestre l'API, la base de données
 *          et le serveur WebSocket
 * @version 4.0
 * @date 22/05/2026
 * @author Guerriau Lucien
 */

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

/**
 * @class aprsfi
 * @brief Fenêtre principale de l'application APRS.fi
 * @details Coordonne les trois composants principaux :
 *          - ApiClient pour les requêtes APRS.fi (positions et météo)
 *          - DatabaseManager pour la persistance MySQL
 *          - WebSocketServer pour la diffusion temps réel aux clients web
 */
class aprsfi : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructeur — initialise les composants et charge la configuration
     * @param parent Widget parent Qt
     */
    explicit aprsfi(QWidget *parent = nullptr);

    /**
     * @brief Destructeur
     */
    ~aprsfi();

private slots:
    /**
     * @brief Slot du bouton "Lancer le serveur"
     * @details Connecte la BDD, démarre le WebSocket et lance les deux clients API
     */
    void on_LancerServeur_clicked();

    /**
     * @brief Slot du bouton "Forcer l'API"
     * @details Déclenche immédiatement une requête manuelle pour les deux types de données
     */
    void on_ForcerAPI_clicked();

    /**
     * @brief Slot de réception des données API parsées
     * @details Redirige vers saveTelemetry ou saveEntry selon le type, puis broadcast
     * @param entries Tableau JSON des entrées reçues
     * @param type Type de données : correspond à apiWhat ou apiWhatWx
     */
    void onApiDataReceived(const QJsonArray &entries, const QString &type);

    /**
     * @brief Slot de réception de la réponse brute JSON
     * @details Affiche la réponse dans le panneau RepAPI avec un en-tête selon le type
     * @param json Réponse JSON brute formatée
     * @param type Type de données : correspond à apiWhat ou apiWhatWx
     */
    void onApiRawResponse(const QString &json, const QString &type);

    /**
     * @brief Slot de réception des messages de log des composants
     * @param message Texte informatif à afficher
     */
    void onLogMessage(const QString &message);

    /**
     * @brief Slot de réception des messages d'erreur des composants
     * @param error Texte d'erreur à afficher
     */
    void onErrorMessage(const QString &error);

private:
    /** @brief Interface graphique générée par Qt Designer */
    Ui::aprsfi *ui;

    /** @brief Gestionnaire de base de données */
    DatabaseManager *database;

    /** @brief Client API pour les données de position (loc) */
    ApiClient *apiClient;

    /** @brief Client API pour les données météo (wx) */
    ApiClient *apiClientWx;

    /** @brief Serveur WebSocket de diffusion */
    WebSocketServer *wsServer;

    /** @brief Indique si le serveur est en cours d'exécution */
    bool isRunning;

    /** @brief Objet de lecture du fichier config.ini */
    QSettings *settings;

    /** @brief Indicatif APRS interrogé */
    QString apiName;

    /** @brief Type de données position (ex: "loc") */
    QString apiWhat;

    /** @brief Type de données météo (ex: "wx") */
    QString apiWhatWx;

    /** @brief Clé d'accès à l'API APRS.fi */
    QString apiKey;

    /** @brief Format de réponse API (json) */
    QString apiFormat;

    /** @brief Intervalle de polling en millisecondes */
    int apiInterval;

    /**
     * @brief Charge tous les paramètres depuis config.ini
     * @details Délègue également loadConfig à DatabaseManager et WebSocketServer
     */
    void loadSettings();

    /**
     * @brief Affiche un message horodaté dans le panneau de logs
     * @param message Texte à afficher
     * @param isError Si true, le message est préfixé [ERREUR], sinon [INFO]
     */
    void logToUI(const QString &message, bool isError = false);

    /**
     * @brief Met à jour l'état actif/inactif des boutons selon isRunning
     */
    void updateButtons();
};

#endif // APRSFI_H
