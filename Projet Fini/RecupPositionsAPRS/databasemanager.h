#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>

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

    void loadConfig(QSettings *settings);
    DbStatus connectToDatabase();
    void disconnect();
    DbStatus saveEntry(const QJsonObject &entry);
    DbStatus saveTelemetry(const QJsonObject &entry);
    QJsonArray getCurrentPositions();

signals:
    void logMessage(const QString &message);
    void errorOccurred(const QString &error);

private:
    QSqlDatabase db;
    QString m_host;
    QString m_user;
    QString m_pass;
    QString m_dbName;

    DbStatus insertIntoHistory(const QJsonObject &entry);
    DbStatus updatePosition(const QJsonObject &entry);
};

#endif // DATABASEMANAGER_H
