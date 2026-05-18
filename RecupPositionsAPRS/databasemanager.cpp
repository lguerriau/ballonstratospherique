#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

DbStatus DatabaseManager::connectToDatabase(const QString &host, const QString &user, const QString &password, const QString &dbName) {
    DbStatus status = DB_ERROR_CONNECTION;

    // Gestion de la connexion existante
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        db = QSqlDatabase::database(QSqlDatabase::defaultConnection);
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL");
    }

    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(password);
    db.setDatabaseName(dbName);

    // Power of 10: un seul point de retour, code explicite
    if (db.open()) {
        emit logMessage("Connexion reussie a la BDD : " + dbName + " sur " + host);
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

    // Power of 10: vérification explicite au lieu de early return
    if (db.isOpen()) {
        DbStatus historyStatus = insertIntoHistory(entry);
        DbStatus positionStatus = updatePosition(entry);

        // Les deux opérations doivent réussir
        if (historyStatus == DB_SUCCESS && positionStatus == DB_SUCCESS) {
            emit logMessage("Sauvegarde BDD OK pour : " + entry.value("name").toString());
            status = DB_SUCCESS;
        } else {
            // Propagation du premier code d'erreur rencontré
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

    // Vérifier si l'entrée existe
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM POSITION WHERE name = :name");
    checkQuery.bindValue(":name", name);

    if (checkQuery.exec() && checkQuery.next()) {
        int exists = checkQuery.value(0).toInt();
        QSqlQuery posQuery(db);

        // Préparation de la requête selon l'existence
        if (exists > 0) {
            posQuery.prepare("UPDATE POSITION SET lat = :lat, lng = :lng, lasttime = :lasttime WHERE name = :name");
        } else {
            posQuery.prepare("INSERT INTO POSITION (name, lat, lng, lasttime) VALUES (:name, :lat, :lng, :lasttime)");
        }

        posQuery.bindValue(":name", name);
        posQuery.bindValue(":lat", entry.value("lat").toString());
        posQuery.bindValue(":lng", entry.value("lng").toString());
        posQuery.bindValue(":lasttime", entry.value("lasttime").toString());

        if (posQuery.exec()) {
            status = DB_SUCCESS;
        } else {
            emit errorOccurred("Erreur update POSITION : " + posQuery.lastError().text());
            status = DB_ERROR_UPDATE;
        }
    } else {
        emit errorOccurred("Erreur verification POSITION : " + checkQuery.lastError().text());
        status = DB_ERROR_UPDATE;
    }

    return status;
}

DbStatus DatabaseManager::saveTelemetry(const QJsonObject &entry) {
    DbStatus status = DB_ERROR_NOT_CONNECTED;

    if (db.isOpen()) {
        QSqlQuery query(db);
        // Utilisation de REPLACE INTO pour éviter les erreurs de clé primaire (doublons)
        query.prepare("REPLACE INTO TELEMETRIE (id_telemetrie, name, time, temp, pressure, humidity, wind_direction, wind_speed) "
                      "VALUES (:id_telemetrie, :name, :time, :temp, :pressure, :humidity, :wind_direction, :wind_speed)");

        QString name = entry.value("name").toVariant().toString();
        QString timeStr = entry.value("time").toVariant().toString();
        QString idTelemetrie = name + "_" + timeStr;

        query.bindValue(":id_telemetrie", idTelemetrie);
        query.bindValue(":name", name);
        query.bindValue(":time", timeStr);
        query.bindValue(":temp", entry.value("temp").toVariant().toString());
        query.bindValue(":pressure", entry.value("pressure").toVariant().toString());
        query.bindValue(":humidity", entry.value("humidity").toVariant().toString());
        query.bindValue(":wind_direction", entry.value("wind_direction").toVariant().toString());
        query.bindValue(":wind_speed", entry.value("wind_speed").toVariant().toString());

        if (query.exec()) {
            emit logMessage("Sauvegarde Telemetrie OK pour : " + name);
            status = DB_SUCCESS;
        } else {
            emit errorOccurred("Erreur SQL TELEMETRIE : " + query.lastError().text());
            status = DB_ERROR_INSERT;
        }
    }

    return status;
}

QJsonArray DatabaseManager::getCurrentPositions() {
    QJsonArray positions;

    if (db.isOpen()) {
        QSqlQuery query(db);

        if (query.exec("SELECT name, lat, lng FROM POSITION")) {
            while (query.next()) {
                QJsonObject pos;
                pos["name"] = query.value("name").toString();
                pos["lat"] = query.value("lat").toDouble();
                pos["lng"] = query.value("lng").toDouble();
                positions.append(pos);
            }
        }
    }

    return positions;
}
