/**
 * @file databasemanager.cpp
 * @brief Implémentation de la classe DatabaseManager
 * @details Gestion de la connexion et des opérations sur la base de données MySQL
 * @version 4.0
 * @date 22/05/2026
 * @author Guerriau Lucien
 */

#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

/**
 * @brief Constructeur de DatabaseManager
 * @param parent Objet parent Qt
 */
DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

/**
 * @brief Destructeur — ferme la connexion si ouverte
 */
DatabaseManager::~DatabaseManager() {
    disconnect();
}

/**
 * @brief Charge les paramètres de connexion depuis config.ini
 * @param settings Pointeur vers l'objet QSettings du fichier config.ini
 */
void DatabaseManager::loadConfig(QSettings *settings) {
    m_host = settings->value("Database/host").toString();
    m_user = settings->value("Database/username").toString();
    m_pass = settings->value("Database/password").toString();
    m_dbName = settings->value("Database/database").toString();
}

/**
 * @brief Établit la connexion à la base de données MySQL
 * @details Réutilise la connexion par défaut si elle existe déjà, sinon en crée une nouvelle.
 * @return DB_SUCCESS si la connexion est établie, DB_ERROR_CONNECTION sinon
 */
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

/**
 * @brief Ferme la connexion à la base de données
 */
void DatabaseManager::disconnect() {
    if (db.isOpen()) {
        db.close();
        emit logMessage("Deconnexion de la base de donnees");
    }
}

/**
 * @brief Sauvegarde une entrée de position APRS complète
 * @details Appelle insertIntoHistory et updatePosition. Retourne le premier statut
 *          d'erreur rencontré, ou DB_SUCCESS si les deux réussissent.
 * @param entry Objet JSON contenant les champs de la trame APRS
 * @return Code DbStatus correspondant au résultat global
 */
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

/**
 * @brief Insère une trame APRS dans la table HISTORIQUE
 * @param entry Objet JSON de la trame APRS (name, type, time, lasttime, lat, lng, ...)
 * @return DB_SUCCESS si l'insertion réussit, DB_ERROR_INSERT sinon
 */
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

/**
 * @brief Met à jour ou crée l'entrée de position dans la table POSITIONS
 * @details Vérifie l'existence de la station. Si elle existe : UPDATE, sinon : INSERT.
 * @param entry Objet JSON contenant name, lat, lng, lasttime
 * @return DB_SUCCESS si l'opération réussit, DB_ERROR_UPDATE sinon
 */
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

/**
 * @brief Sauvegarde une entrée de télémétrie météo
 * @details Utilise REPLACE INTO pour éviter les doublons sur la clé composée name+time.
 *          L'id_telemetrie est construit sous la forme "name_time".
 * @param entry Objet JSON contenant name, time, temp, pressure, humidity
 * @return DB_SUCCESS si la sauvegarde réussit, DB_ERROR_INSERT ou DB_ERROR_NOT_CONNECTED sinon
 */
DbStatus DatabaseManager::saveTelemetry(const QJsonObject &entry) {
    DbStatus status = DB_ERROR_NOT_CONNECTED;

    if (db.isOpen()) {
        QSqlQuery query(db);
        query.prepare("REPLACE INTO TELEMETRIES (id_telemetrie, name, time, temp, pressure, humidity) "
                      "VALUES (:id_telemetrie, :name, :time, :temp, :pressure, :humidity)");

        QString name = entry.value("name").toString();
        QString timeStr = entry.value("time").toString();
        QString idTelemetrie = name + "_" + timeStr;

        query.bindValue(":id_telemetrie", idTelemetrie);
        query.bindValue(":name", name);
        query.bindValue(":time", timeStr);
        query.bindValue(":temp", entry.value("temp").toVariant().toString());
        query.bindValue(":pressure", entry.value("pressure").toVariant().toString());
        query.bindValue(":humidity", entry.value("humidity").toVariant().toString());

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

/**
 * @brief Récupère les dernières positions connues avec leur télémétrie associée
 * @details Jointure LEFT JOIN entre POSITIONS et la dernière entrée TELEMETRIES
 *          de chaque station, identifiée par MAX(time).
 * @return Tableau JSON des positions avec champs : name, lat, lng, temp, pressure, humidity
 */
QJsonArray DatabaseManager::getCurrentPositions() {
    QJsonArray positions;

    if (db.isOpen()) {
        QSqlQuery query(db);
        QString sql = "SELECT p.name, p.lat, p.lng, t.temp, t.pressure, t.humidity "
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
                positions.append(pos);
            }
        }
    }
    return positions;
}
