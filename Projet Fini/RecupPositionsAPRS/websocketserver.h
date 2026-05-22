/**
 * @file websocketserver.h
 * @brief Déclaration de la classe WebSocketServer
 * @details Serveur WebSocket pour diffuser les positions APRS aux clients web
 * @version 4.0
 * @date 22/05/2026
 * @author Guerriau Lucien
 */

#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QList>
#include <QJsonArray>
#include <QSettings>

/**
 * @brief Codes de retour des opérations du serveur WebSocket
 */
enum WsStatus {
    WS_SUCCESS = 0,              ///< Démarrage réussi
    WS_ERROR_ALREADY_RUNNING = 1, ///< Le serveur est déjà en écoute
    WS_ERROR_START_FAILED = 2    ///< Échec du démarrage (port occupé, etc.)
};

/**
 * @class WebSocketServer
 * @brief Serveur WebSocket de diffusion des positions APRS
 * @details Gère les connexions entrantes des clients web et leur diffuse
 *          en temps réel les mises à jour de position et de télémétrie
 *          au format JSON.
 */
class WebSocketServer : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructeur de WebSocketServer
     * @param parent Objet parent Qt
     */
    explicit WebSocketServer(QObject *parent = nullptr);

    /**
     * @brief Destructeur — arrête le serveur et déconnecte les clients
     */
    ~WebSocketServer();

    /**
     * @brief Charge le port d'écoute depuis le fichier de configuration
     * @param settings Pointeur vers l'objet QSettings du fichier config.ini
     */
    void loadConfig(QSettings *settings);

    /**
     * @brief Démarre le serveur WebSocket en écoute
     * @return WS_SUCCESS si le serveur démarre, WsStatus d'erreur sinon
     */
    WsStatus start();

    /**
     * @brief Arrête le serveur et déconnecte tous les clients
     */
    void stop();

    /**
     * @brief Diffuse les positions courantes à tous les clients connectés
     * @details Envoie un message JSON par position avec les champs :
     *          type, name, lat, lng, temp, pressure, humidity
     * @param positions Tableau JSON des positions issues de DatabaseManager
     */
    void broadcastPositions(const QJsonArray &positions);

signals:
    /**
     * @brief Émis pour journaliser un événement informationnel
     * @param message Texte du message de log
     */
    void logMessage(const QString &message);

    /**
     * @brief Émis en cas d'erreur du serveur WebSocket
     * @param error Description de l'erreur
     */
    void errorOccurred(const QString &error);

private slots:
    /**
     * @brief Slot appelé lors d'une nouvelle connexion cliente
     * @details Enregistre le socket et connecte ses signaux
     */
    void onNewConnection();

    /**
     * @brief Slot appelé à la réception d'un message texte d'un client
     * @param message Contenu du message reçu
     */
    void onTextMessageReceived(const QString &message);

    /**
     * @brief Slot appelé lors de la déconnexion d'un client
     * @details Retire le socket de la liste et le supprime
     */
    void onSocketDisconnected();

private:
    /** @brief Instance du serveur WebSocket Qt */
    QWebSocketServer *server;

    /** @brief Liste des sockets clients connectés */
    QList<QWebSocket*> clients;

    /** @brief Port TCP d'écoute du serveur */
    int m_port;
};

#endif // WEBSOCKETSERVER_H
