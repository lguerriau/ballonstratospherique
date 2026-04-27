#include <QtTest>
#include <QSignalSpy>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include "databasemanager.h"

class TestDatabaseManager : public QObject
{
    Q_OBJECT

private slots:
    // Initialisation et Nettoyage
    void initTestCase();
    void cleanup();

    // --- CATEGORIE 1 : CONNEXION ET ETATS ---
    void test1_1_InitialState();
    void test1_2_ConnectionInvalidIP();
    void test1_3_ConnectionInvalidCreds();
    void test1_4_ConnectionInvalidDB();
    void test1_5_ConnectionValid();
    void test1_6_DisconnectPropre();
    void test1_7_DisconnectRedondant();

    // --- CATEGORIE 2 : SECURITE ET ROBUSTESSE ---
    void test2_1_ReadOffline();
    void test2_2_WriteOffline();
    void test2_3_EmptyJson();
    void test2_4_IncompleteData();
    void test2_5_SqlInjection();

    // --- CATEGORIE 3 : LOGIQUE METIER ---
    void test3_1_ReadEmptyDB();
    void test3_2_FirstInsertion();
    void test3_3_ReadAfterInsertion();
    void test3_4_UpdateAndHistoryLogic();
    void test3_5_MultiTargets();
    void test3_6_ReadMultiTargets();

private:
    DatabaseManager *db;

    // CONFIGURATION (A ajuster selon ton environnement)
    const QString HOST = "172.18.58.85"; // L'IP de ton serveur MariaDB
    const QString USER = "root";
    const QString PASS = "toto";
    const QString DBNAME = "ballon2026_test"; // Base de données de TEST uniquement

    void truncateTables();
};

// ==========================================================
// INITIALISATION ET UTILITAIRES
// ==========================================================

void TestDatabaseManager::initTestCase() {
    db = new DatabaseManager(this);
}

void TestDatabaseManager::cleanup() {
    // On vide la base après chaque test pour ne pas fausser le suivant
    if (db->isConnected()) {
        truncateTables();
    }
}

void TestDatabaseManager::truncateTables() {
    QSqlQuery q;
    q.exec("TRUNCATE TABLE POSITION");
    q.exec("TRUNCATE TABLE HISTORIQUE");
}

// ==========================================================
// CATEGORIE 1 : CONNEXION
// ==========================================================

void TestDatabaseManager::test1_1_InitialState() {
    QCOMPARE(db->isConnected(), false);
}

void TestDatabaseManager::test1_2_ConnectionInvalidIP() {
    QSignalSpy spy(db, &DatabaseManager::errorOccurred);
    bool ok = db->connect("0.0.0.0", USER, PASS, DBNAME);
    QCOMPARE(ok, false);
    QVERIFY(spy.count() > 0);
}

void TestDatabaseManager::test1_3_ConnectionInvalidCreds() {
    bool ok = db->connect(HOST, "user_inexistant", "mauvais_pass", DBNAME);
    QCOMPARE(ok, false);
}

void TestDatabaseManager::test1_4_ConnectionInvalidDB() {
    bool ok = db->connect(HOST, USER, PASS, "base_qui_n_existe_pas");
    QCOMPARE(ok, false);
}

void TestDatabaseManager::test1_5_ConnectionValid() {
    bool ok = db->connect(HOST, USER, PASS, DBNAME);
    QCOMPARE(ok, true);
    QCOMPARE(db->isConnected(), true);
}

void TestDatabaseManager::test1_6_DisconnectPropre() {
    db->connect(HOST, USER, PASS, DBNAME);
    db->disconnect();
    QCOMPARE(db->isConnected(), false);
}

void TestDatabaseManager::test1_7_DisconnectRedondant() {
    db->disconnect();
    db->disconnect(); // Ne doit pas crasher
    QCOMPARE(db->isConnected(), false);
}

// ==========================================================
// CATEGORIE 2 : ROBUSTESSE
// ==========================================================

void TestDatabaseManager::test2_1_ReadOffline() {
    db->disconnect();
    QSignalSpy spy(db, &DatabaseManager::errorOccurred);
    QJsonArray res = db->getCurrentPositions();
    QCOMPARE(res.size(), 0);
    QCOMPARE(spy.count(), 1);
}

void TestDatabaseManager::test2_2_WriteOffline() {
    db->disconnect();
    QJsonObject obj;
    obj["name"] = "TEST";
    bool ok = db->saveEntry(obj);
    QCOMPARE(ok, false);
}

void TestDatabaseManager::test2_3_EmptyJson() {
    db->connect(HOST, USER, PASS, DBNAME);
    QJsonObject empty;
    bool ok = db->saveEntry(empty);
    QCOMPARE(ok, true); // Doit passer sans crash
}

void TestDatabaseManager::test2_4_IncompleteData() {
    db->connect(HOST, USER, PASS, DBNAME);
    QJsonObject incomplete;
    incomplete["name"] = "PARTIEL";
    // Manque lat, lng, time...
    QVERIFY(db->saveEntry(incomplete));
}

void TestDatabaseManager::test2_5_SqlInjection() {
    db->connect(HOST, USER, PASS, DBNAME);
    QJsonObject hack;
    hack["name"] = "HACKER'; DROP TABLE POSITION;--";
    QVERIFY(db->saveEntry(hack));

    // Vérification que la table existe toujours
    QSqlQuery q("SELECT COUNT(*) FROM POSITION");
    QVERIFY(q.exec());
}

// ==========================================================
// CATEGORIE 3 : LOGIQUE METIER
// ==========================================================

void TestDatabaseManager::test3_1_ReadEmptyDB() {
    db->connect(HOST, USER, PASS, DBNAME);
    truncateTables();
    QCOMPARE(db->getCurrentPositions().size(), 0);
}

void TestDatabaseManager::test3_2_FirstInsertion() {
    db->connect(HOST, USER, PASS, DBNAME);
    QJsonObject entry;
    entry["name"] = "BALLON-1";
    entry["lat"] = "48.5";
    entry["lng"] = "2.3";

    QVERIFY(db->saveEntry(entry));

    QSqlQuery q1("SELECT COUNT(*) FROM POSITION"); q1.next();
    QCOMPARE(q1.value(0).toInt(), 1);

    QSqlQuery q2("SELECT COUNT(*) FROM HISTORIQUE"); q2.next();
    QCOMPARE(q2.value(0).toInt(), 1);
}

void TestDatabaseManager::test3_3_ReadAfterInsertion() {
    db->connect(HOST, USER, PASS, DBNAME);
    QJsonArray res = db->getCurrentPositions();
    QCOMPARE(res.size(), 1);
    QCOMPARE(res[0].toObject()["name"].toString(), QString("BALLON-1"));
}

void TestDatabaseManager::test3_4_UpdateAndHistoryLogic() {
    db->connect(HOST, USER, PASS, DBNAME);

    // On envoie une 2ème position pour le même nom
    QJsonObject move;
    move["name"] = "BALLON-1";
    move["lat"] = "49.1";
    db->saveEntry(move);

    // POSITION : doit rester à 1 ligne (le dernier UPDATE)
    QSqlQuery qPos("SELECT lat FROM POSITION WHERE name = 'BALLON-1'");
    qPos.next();
    QCOMPARE(qPos.value(0).toDouble(), 49.1);

    // HISTORIQUE : doit passer à 2 lignes
    QSqlQuery qHist("SELECT COUNT(*) FROM HISTORIQUE WHERE name = 'BALLON-1'");
    qHist.next();
    QCOMPARE(qHist.value(0).toInt(), 2);
}

void TestDatabaseManager::test3_5_MultiTargets() {
    db->connect(HOST, USER, PASS, DBNAME);
    QJsonObject target2;
    target2["name"] = "AVION-99";
    db->saveEntry(target2);

    QSqlQuery q("SELECT COUNT(*) FROM POSITION");
    q.next();
    QCOMPARE(q.value(0).toInt(), 2); // BALLON-1 + AVION-99
}

void TestDatabaseManager::test3_6_ReadMultiTargets() {
    db->connect(HOST, USER, PASS, DBNAME);
    QJsonArray res = db->getCurrentPositions();
    QCOMPARE(res.size(), 2);
}

QTEST_MAIN(TestDatabaseManager)
#include "tst_main.moc"
