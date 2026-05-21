#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

void DatabaseManager::loadConfig(QSettings *settings) {
    m_host = settings->value("Database/host").toString();
    m_user = settings->value("Database/username").toString();
    m_pass = settings->value("Database/password").toString();
    m_dbName = settings->value("Database/database").toString();
}

DbStatus DatabaseManager::connectToDatabase() {
    DbStatus status = DB_ERROR_CONNECTION;

    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        db = QSqlDatabase::database(QSqlDatabase::defaultConnection);
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL");
    }

    db.setHostName(m_host);
    db.setUserName(m_user);
    db.setPassword(m_pass);
    db.setDatabaseName(m_dbName);

    if (db.open()) {
        emit logMessage("Connexion reussie a la BDD : " + m_dbName + " sur " + m_host);
        status = DB_SUCCESS;
    } else {
        emit errorOccurred("Erreur de connexion BDD : " + db.lastError().text());
        status = DB_ERROR_CONNECTION;
    }

    return status;
}

void DatabaseManager::disconnect() {
    if (db.isOpen()) {
        db.close();
        emit logMessage("Deconnexion de la base de donnees");
    }
}

DbStatus DatabaseManager::saveEntry(const QJsonObject &entry) {
    DbStatus status = DB_ERROR_NOT_CONNECTED;

    if (db.isOpen()) {
        DbStatus historyStatus = insertIntoHistory(entry);
        DbStatus positionStatus = updatePosition(entry);

        if (historyStatus == DB_SUCCESS && positionStatus == DB_SUCCESS) {
            emit logMessage("Sauvegarde BDD OK pour : " + entry.value("name").toString());
            status = DB_SUCCESS;
        } else {
            status = (historyStatus != DB_SUCCESS) ? historyStatus : positionStatus;
        }
    }
    return status;
}

DbStatus DatabaseManager::insertIntoHistory(const QJsonObject &entry) {
    DbStatus status = DB_ERROR_INSERT;

    QSqlQuery query(db);
    query.prepare("INSERT INTO HISTORIQUE (name, type, time, lasttime, lat, lng, symbol, srccall, dstcall, phg, comment, path) "
                  "VALUES (:name, :type, :time, :lasttime, :lat, :lng, :symbol, :srccall, :dstcall, :phg, :comment, :path)");

    query.bindValue(":name", entry.value("name").toString());
    query.bindValue(":type", entry.value("type").toString());
    query.bindValue(":time", entry.value("time").toString());
    query.bindValue(":lasttime", entry.value("lasttime").toString());
    query.bindValue(":lat", entry.value("lat").toString());
    query.bindValue(":lng", entry.value("lng").toString());
    query.bindValue(":symbol", entry.value("symbol").toString());
    query.bindValue(":srccall", entry.value("srccall").toString());
    query.bindValue(":dstcall", entry.value("dstcall").toString());
    query.bindValue(":phg", entry.value("phg").toString());
    query.bindValue(":comment", entry.value("comment").toString());
    query.bindValue(":path", entry.value("path").toString());

    if (query.exec()) {
        status = DB_SUCCESS;
    } else {
        emit errorOccurred("Erreur insertion HISTORIQUE : " + query.lastError().text());
        status = DB_ERROR_INSERT;
    }
    return status;
}

DbStatus DatabaseManager::updatePosition(const QJsonObject &entry) {
    DbStatus status = DB_ERROR_UPDATE;
    QString name = entry.value("name").toString();

    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM POSITIONS WHERE name = :name");
    checkQuery.bindValue(":name", name);

    if (checkQuery.exec() && checkQuery.next()) {
        int exists = checkQuery.value(0).toInt();
        QSqlQuery posQuery(db);

        if (exists > 0) {
            posQuery.prepare("UPDATE POSITIONS SET lat = :lat, lng = :lng, lasttime = :lasttime WHERE name = :name");
        } else {
            posQuery.prepare("INSERT INTO POSITIONS (name, lat, lng, lasttime) VALUES (:name, :lat, :lng, :lasttime)");
        }

        posQuery.bindValue(":name", name);
        posQuery.bindValue(":lat", entry.value("lat").toString());
        posQuery.bindValue(":lng", entry.value("lng").toString());
        posQuery.bindValue(":lasttime", entry.value("lasttime").toString());

        if (posQuery.exec()) {
            status = DB_SUCCESS;
        } else {
            emit errorOccurred("Erreur update POSITIONS : " + posQuery.lastError().text());
            status = DB_ERROR_UPDATE;
        }
    } else {
        emit errorOccurred("Erreur verification POSITIONS : " + checkQuery.lastError().text());
        status = DB_ERROR_UPDATE;
    }
    return status;
}

DbStatus DatabaseManager::saveTelemetry(const QJsonObject &entry) {
    DbStatus status = DB_ERROR_NOT_CONNECTED;

    if (db.isOpen()) {
        QSqlQuery query(db);
        // query.prepare("REPLACE INTO TELEMETRIES (id_telemetrie, name, time, temp, pressure, humidity, wind_direction, wind_speed) "
        //               "VALUES (:id_telemetrie, :name, :time, :temp, :pressure, :humidity, :wind_direction, :wind_speed)");

        query.prepare("REPLACE INTO TELEMETRIES (id_telemetrie, name, time, temp, pressure, humidity) "
                      "VALUES (:id_telemetrie, :name, :time, :temp, :pressure, :humidity) ; ");

        QString name = entry.value("name").toString();
        QString timeStr = entry.value("time").toString();
        QString idTelemetrie = name + "_" + timeStr;

        query.bindValue(":id_telemetrie", idTelemetrie);
        query.bindValue(":name", name);
        query.bindValue(":time", timeStr);
        query.bindValue(":temp", entry.value("temp").toVariant().toString());
        query.bindValue(":pressure", entry.value("pressure").toVariant().toString());
        query.bindValue(":humidity", entry.value("humidity").toVariant().toString());
      /*  query.bindValue(":wind_direction", entry.value("wind_direction").toVariant().toString());
        query.bindValue(":wind_speed", entry.value("wind_speed").toVariant().toString());*/

        if (query.exec()) {
            emit logMessage("Sauvegarde Telemetrie OK pour : " + name);
            status = DB_SUCCESS;
        } else {
            emit errorOccurred("Erreur SQL TELEMETRIES : " + query.lastError().text());
            status = DB_ERROR_INSERT;
        }
    }
    return status;
}

QJsonArray DatabaseManager::getCurrentPositions() {
    QJsonArray positions;

    if (db.isOpen()) {
        QSqlQuery query(db);
        QString sql = "SELECT p.name, p.lat, p.lng, t.temp, t.pressure, t.humidity, t.wind_direction, t.wind_speed "
                      "FROM POSITIONS p "
                      "LEFT JOIN TELEMETRIES t ON p.name = t.name "
                      "AND t.time = (SELECT MAX(time) FROM TELEMETRIES WHERE name = p.name)";

        if (query.exec(sql)) {
            while (query.next()) {
                QJsonObject pos;
                pos["name"] = query.value(0).toString();
                pos["lat"] = query.value(1).toDouble();
                pos["lng"] = query.value(2).toDouble();
                pos["temp"] = query.value(3).toString();
                pos["pressure"] = query.value(4).toString();
                pos["humidity"] = query.value(5).toString();
                pos["wind_direction"] = query.value(6).toString();
                pos["wind_speed"] = query.value(7).toString();
                positions.append(pos);
            }
        }
    }
    return positions;
}
