#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QJsonArray>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool connect(const QString &host, const QString &user, const QString &password, const QString &dbName);
    void disconnect();
    bool isConnected() const;
    bool saveEntry(const QJsonObject &entry);
    QJsonArray getCurrentPositions();

signals:
    void logMessage(const QString &message);
    void errorOccurred(const QString &error);

private:
    QSqlDatabase db;
    bool insertIntoHistory(const QJsonObject &entry);
    bool updatePosition(const QJsonObject &entry);
};

#endif // DATABASEMANAGER_H
