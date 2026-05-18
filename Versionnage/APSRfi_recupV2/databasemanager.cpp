#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonArray>
#include <QVariant>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent) {
}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

bool DatabaseManager::connect(const QString &host, const QString &user,
                               const QString &password, const QString &dbName) {
    // Création de la connexion MySQL/MariaDB
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(password);
    db.setDatabaseName(dbName);

    if (db.open()) {
        emit logMessage("✓ Connexion réussie à la base de données : " + dbName);
        return true;
    } else {
        emit errorOccurred("✗ Erreur de connexion BDD : " + db.lastError().text());
        return false;
    }
}

void DatabaseManager::disconnect() {
    if (db.isOpen()) {
        db.close();
        emit logMessage("Déconnexion de la base de données");
    }
}

bool DatabaseManager::isConnected() const {
    return db.isOpen();
}

bool DatabaseManager::saveEntry(const QJsonObject &entry) {
    if (!isConnected()) {
        emit errorOccurred("Impossible de sauvegarder : BDD non connectée");
        return false;
    }

    // Sauvegarde dans HISTORIQUE et POSITION
    bool historyOk = insertIntoHistory(entry);
    bool positionOk = updatePosition(entry);

    if (historyOk && positionOk) {
        QString name = entry.value("name").toString();
        emit logMessage("✓ Sauvegardé : " + name);
        return true;
    }

    return false;
}

bool DatabaseManager::insertIntoHistory(const QJsonObject &entry) {
    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO HISTORIQUE "
        "(name, type, time, lasttime, lat, lng, symbol, srccall, dstcall, phg, comment, path) "
        "VALUES "
        "(:name, :type, :time, :lasttime, :lat, :lng, :symbol, :srccall, :dstcall, :phg, :comment, :path)"
    );

    // Liaison des valeurs
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

    if (!query.exec()) {
        emit errorOccurred("Erreur insertion HISTORIQUE : " + query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseManager::updatePosition(const QJsonObject &entry) {
    QString name = entry.value("name").toString();

    // Vérifier si l'indicatif existe déjà
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM POSITION WHERE name = :name");
    checkQuery.bindValue(":name", name);

    if (!checkQuery.exec() || !checkQuery.next()) {
        emit errorOccurred("Erreur vérification POSITION : " + checkQuery.lastError().text());
        return false;
    }

    int exists = checkQuery.value(0).toInt();
    QSqlQuery posQuery(db);

    if (exists > 0) {
        // UPDATE : l'indicatif existe déjà
        posQuery.prepare(
            "UPDATE POSITION SET lat = :lat, lng = :lng, lasttime = :lasttime "
            "WHERE name = :name"
        );
    } else {
        // INSERT : nouvel indicatif
        posQuery.prepare(
            "INSERT INTO POSITION (name, lat, lng, lasttime) "
            "VALUES (:name, :lat, :lng, :lasttime)"
        );
    }

    // Liaison des valeurs
    posQuery.bindValue(":name", name);
    posQuery.bindValue(":lat", entry.value("lat").toString());
    posQuery.bindValue(":lng", entry.value("lng").toString());
    posQuery.bindValue(":lasttime", entry.value("lasttime").toString());

    if (!posQuery.exec()) {
        emit errorOccurred("Erreur mise à jour POSITION : " + posQuery.lastError().text());
        return false;
    }

    return true;
}

QJsonArray DatabaseManager::getCurrentPositions() {
    QJsonArray positions;

    if (!isConnected()) {
        emit errorOccurred("Impossible de lire les positions : BDD non connectée");
        return positions;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT name, lat, lng, lasttime FROM POSITION")) {
        emit errorOccurred("Erreur lecture POSITION : " + query.lastError().text());
        return positions;
    }

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
