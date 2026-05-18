#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QJsonArray>

// Codes de retour explicites au lieu de bool
enum DbStatus {
    DB_SUCCESS = 0,
    DB_ERROR_CONNECTION = 1,
    DB_ERROR_INSERT = 2,
    DB_ERROR_UPDATE = 3,
    DB_ERROR_NOT_CONNECTED = 4
};

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    DbStatus connectToDatabase(const QString &host, const QString &user, const QString &password, const QString &dbName);
    void disconnect();
    // SUPPRIMÉ: isConnected() - redondant avec db.isOpen()
    DbStatus saveEntry(const QJsonObject &entry);
    DbStatus saveTelemetry(const QJsonObject &entry);
    QJsonArray getCurrentPositions();

signals:
    void logMessage(const QString &message);
    void errorOccurred(const QString &error);

private:
    QSqlDatabase db;
    DbStatus insertIntoHistory(const QJsonObject &entry);
    DbStatus updatePosition(const QJsonObject &entry);
};

#endif // DATABASEMANAGER_H
