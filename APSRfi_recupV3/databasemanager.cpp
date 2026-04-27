#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonArray>
#include <QVariant>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

bool DatabaseManager::connect(const QString &host, const QString &user, const QString &password, const QString &dbName) {
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(password);
    db.setDatabaseName(dbName);

    if (db.open()) {
        emit logMessage("Connecte a la base de donnees " + dbName + ".");
        return true;
    } else {
        emit errorOccurred("Erreur de connexion BDD : " + db.lastError().text());
        return false;
    }
}

void DatabaseManager::disconnect() {
    if (db.isOpen()) {
        db.close();
        emit logMessage("Deconnecte de la base de donnees.");
    }
}

bool DatabaseManager::isConnected() const {
    return db.isOpen();
}

bool DatabaseManager::saveEntry(const QJsonObject &entry) {
    if (!isConnected()) return false;

    bool historyOk = insertIntoHistory(entry);
    bool positionOk = updatePosition(entry);

    if (historyOk && positionOk) {
        emit logMessage("Donnees enregistrees pour : " + entry["name"].toString());
        return true;
    }
    return false;
}

bool DatabaseManager::insertIntoHistory(const QJsonObject &entry) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO HISTORIQUE (name, type, time, lasttime, lat, lng, symbol, srccall, dstcall, phg, comment, path) "
                  "VALUES (:name, :type, :time, :lasttime, :lat, :lng, :symbol, :srccall, :dstcall, :phg, :comment, :path)");

    query.bindValue(":name", entry["name"].toString());
    query.bindValue(":type", entry["type"].toString());
    query.bindValue(":time", entry["time"].toString());
    query.bindValue(":lasttime", entry["lasttime"].toString());
    query.bindValue(":lat", entry["lat"].toString());
    query.bindValue(":lng", entry["lng"].toString());
    query.bindValue(":symbol", entry["symbol"].toString());
    query.bindValue(":srccall", entry["srccall"].toString());
    query.bindValue(":dstcall", entry["dstcall"].toString());
    query.bindValue(":phg", entry["phg"].toString());
    query.bindValue(":comment", entry["comment"].toString());
    query.bindValue(":path", entry["path"].toString());

    if (!query.exec()) {
        emit errorOccurred("Erreur insertion historique : " + query.lastError().text());
        return false;
    }
    return true;
}

bool DatabaseManager::updatePosition(const QJsonObject &entry) {
    QString name = entry["name"].toString();

    QSqlQuery check(db);
    check.prepare("SELECT COUNT(*) FROM POSITION WHERE name = :name");
    check.bindValue(":name", name);
    check.exec();
    check.next();

    QSqlQuery posQuery(db);
    if (check.value(0).toInt() > 0) {
        posQuery.prepare("UPDATE POSITION SET lat = :lat, lng = :lng, lasttime = :lasttime WHERE name = :name");
    } else {
        posQuery.prepare("INSERT INTO POSITION (name, lat, lng, lasttime) VALUES (:name, :lat, :lng, :lasttime)");
    }

    posQuery.bindValue(":name", name);
    posQuery.bindValue(":lat", entry["lat"].toString());
    posQuery.bindValue(":lng", entry["lng"].toString());
    posQuery.bindValue(":lasttime", entry["lasttime"].toString());

    if (!posQuery.exec()) {
        emit errorOccurred("Erreur mise a jour position : " + posQuery.lastError().text());
        return false;
    }
    return true;
}

QJsonArray DatabaseManager::getCurrentPositions() {
    QJsonArray positions;
    if (!isConnected()) return positions;

    QSqlQuery query(db);
    query.exec("SELECT name, lat, lng, lasttime FROM POSITION");

    while (query.next()) {
        QJsonObject pos;
        pos["name"] = query.value("name").toString();
        pos["lat"] = query.value("lat").toDouble();
        pos["lng"] = query.value("lng").toDouble();
        pos["lasttime"] = query.value("lasttime").toString();
        positions.append(pos);
    }
    return positions;
}
