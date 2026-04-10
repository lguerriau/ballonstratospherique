#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QJsonObject>

/**
 * DatabaseManager - Gère toutes les interactions avec la base de données MariaDB
 *
 * Responsabilités :
 * - Connexion à la BDD
 * - Insertion dans HISTORIQUE (garde tout l'historique)
 * - Mise à jour dans POSITION (dernière position connue)
 * - Récupération des positions actuelles
 */
class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    // Connexion à la base de données
    bool connect(const QString &host, const QString &user,
                 const QString &password, const QString &dbName);

    // Fermeture de la connexion
    void disconnect();

    // Est-on connecté ?
    bool isConnected() const;

    // Sauvegarde une entrée APRS dans la BDD
    bool saveEntry(const QJsonObject &entry);

    // Récupère toutes les dernières positions
    QJsonArray getCurrentPositions();

signals:
    void logMessage(const QString &message);  // Pour envoyer des logs à l'UI
    void errorOccurred(const QString &error); // Pour signaler les erreurs

private:
    QSqlDatabase db;

    // Insertion dans l'historique (toutes les positions)
    bool insertIntoHistory(const QJsonObject &entry);

    // Mise à jour de la table POSITION (dernière position)
    bool updatePosition(const QJsonObject &entry);
};

#endif // DATABASEMANAGER_H
