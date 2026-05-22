/**
 * @file databasemanager.h
 * @brief Déclaration de la classe DatabaseManager
 * @details Gestion de la connexion et des opérations sur la base de données MySQL
 * @version 4.0
 * @date 22/05/2026
 * @author Guerriau Lucien
 */

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>

/**
 * @brief Codes de retour des opérations sur la base de données
 */
enum DbStatus {
    DB_SUCCESS = 0,          ///< Opération réussie
    DB_ERROR_CONNECTION = 1, ///< Échec de connexion à la base de données
    DB_ERROR_INSERT = 2,     ///< Échec d'une insertion SQL
    DB_ERROR_UPDATE = 3,     ///< Échec d'une mise à jour SQL
    DB_ERROR_NOT_CONNECTED = 4 ///< Opération impossible : base non connectée
};

/**
 * @class DatabaseManager
 * @brief Gestionnaire de base de données MySQL pour le projet APRS
 * @details Gère la connexion à la base MySQL et fournit les méthodes d'écriture
 *          pour les tables HISTORIQUE, POSITIONS et TELEMETRIES, ainsi que
 *          la lecture des positions courantes enrichies de télémétrie.
 */
class DatabaseManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructeur de DatabaseManager
     * @param parent Objet parent Qt
     */
    explicit DatabaseManager(QObject *parent = nullptr);

    /**
     * @brief Destructeur — ferme la connexion si ouverte
     */
    ~DatabaseManager();

    /**
     * @brief Charge les paramètres de connexion depuis un fichier de configuration
     * @param settings Pointeur vers l'objet QSettings du fichier config.ini
     */
    void loadConfig(QSettings *settings);

    /**
     * @brief Établit la connexion à la base de données MySQL
     * @return DB_SUCCESS si la connexion est établie, DB_ERROR_CONNECTION sinon
     */
    DbStatus connectToDatabase();

    /**
     * @brief Ferme la connexion à la base de données
     */
    void disconnect();

    /**
     * @brief Sauvegarde une entrée de position APRS
     * @details Insère dans HISTORIQUE et met à jour ou insère dans POSITIONS
     * @param entry Objet JSON contenant les champs de la trame APRS
     * @return Code DbStatus correspondant au résultat de l'opération
     */
    DbStatus saveEntry(const QJsonObject &entry);

    /**
     * @brief Sauvegarde une entrée de télémétrie météo
     * @details Effectue un REPLACE INTO dans la table TELEMETRIES
     * @param entry Objet JSON contenant les champs météo (temp, pressure, humidity)
     * @return Code DbStatus correspondant au résultat de l'opération
     */
    DbStatus saveTelemetry(const QJsonObject &entry);

    /**
     * @brief Récupère les dernières positions connues avec leur télémétrie associée
     * @details Jointure entre POSITIONS et la dernière entrée TELEMETRIES par station
     * @return Tableau JSON des positions avec champs : name, lat, lng, temp, pressure, humidity
     */
    QJsonArray getCurrentPositions();

signals:
    /**
     * @brief Émis pour journaliser un événement informationnel
     * @param message Texte du message de log
     */
    void logMessage(const QString &message);

    /**
     * @brief Émis en cas d'erreur SQL ou de connexion
     * @param error Description de l'erreur
     */
    void errorOccurred(const QString &error);

private:
    /** @brief Objet de connexion à la base de données */
    QSqlDatabase db;

    /** @brief Hôte du serveur MySQL */
    QString m_host;

    /** @brief Nom d'utilisateur MySQL */
    QString m_user;

    /** @brief Mot de passe MySQL */
    QString m_pass;

    /** @brief Nom de la base de données cible */
    QString m_dbName;

    /**
     * @brief Insère une trame APRS dans la table HISTORIQUE
     * @param entry Objet JSON de la trame APRS
     * @return DB_SUCCESS si l'insertion réussit, DB_ERROR_INSERT sinon
     */
    DbStatus insertIntoHistory(const QJsonObject &entry);

    /**
     * @brief Met à jour ou crée l'entrée de position dans la table POSITIONS
     * @param entry Objet JSON contenant name, lat, lng, lasttime
     * @return DB_SUCCESS si l'opération réussit, DB_ERROR_UPDATE sinon
     */
    DbStatus updatePosition(const QJsonObject &entry);
};

#endif // DATABASEMANAGER_H
