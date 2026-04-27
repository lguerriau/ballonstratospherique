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
    // Exécuté au tout début
    void initTestCase();

    // Exécuté APRES chaque test pour nettoyer la base
    void cleanup();

    // Les tests
    void testInitialState();
    void testInvalidConnection();
    void testValidConnection();
    void testEmptyDatabase();
    void testSaveEntryWithoutConnection();
    void testBasicInsertion();
    void testHistoryAndPositionLogic();
    void testIncompleteData();
    void testSqlInjectionPrevention();
    void testDisconnect();

private:
    DatabaseManager *dbManager;
    // Base de données DÉDIÉE AUX TESTS
    QString testHost = "172.18.58.85";
    QString testUser = "root";
    QString testPass = "toto";
    QString testDb   = "ballon2026";

    void emptyTestTables();
};

void TestDatabaseManager::initTestCase() {
    dbManager = new DatabaseManager(this);
}

// Nettoyage automatique après chaque test
void TestDatabaseManager::cleanup() {
    if (dbManager->isConnected()) {
        emptyTestTables();
    }
}

// Fonction utilitaire pour vider les tables
void TestDatabaseManager::emptyTestTables() {
    QSqlQuery query;
    query.exec("TRUNCATE TABLE POSITION");
    query.exec("TRUNCATE TABLE HISTORIQUE");
}

// --- DÉBUT DES TESTS ---

void TestDatabaseManager::testInitialState() {
    QCOMPARE(dbManager->isConnected(), false);
}

void TestDatabaseManager::testInvalidConnection() {
    QSignalSpy spyError(dbManager, &DatabaseManager::errorOccurred);
    bool result = dbManager->connect(testHost, "mauvais_user", "faux_mdp", testDb);

    QCOMPARE(result, false);
    QCOMPARE(dbManager->isConnected(), false);
    QCOMPARE(spyError.count(), 1);
}

void TestDatabaseManager::testValidConnection() {
    bool result = dbManager->connect(testHost, testUser, testPass, testDb);
    QCOMPARE(result, true);
    QCOMPARE(dbManager->isConnected(), true);
}

void TestDatabaseManager::testEmptyDatabase() {
    dbManager->connect(testHost, testUser, testPass, testDb);
    emptyTestTables(); // On s'assure que c'est vide

    QJsonArray positions = dbManager->getCurrentPositions();
    QCOMPARE(positions.size(), 0); // Doit être vide, pas de crash
}

void TestDatabaseManager::testSaveEntryWithoutConnection() {
    dbManager->disconnect();

    QJsonObject testEntry;
    testEntry["name"] = "F6KNB";

    bool result = dbManager->saveEntry(testEntry);
    QCOMPARE(result, false);
}

void TestDatabaseManager::testBasicInsertion() {
    dbManager->connect(testHost, testUser, testPass, testDb);

    QJsonObject testEntry;
    testEntry["name"] = "TEST-1";
    testEntry["lat"] = "48.8566";
    testEntry["lng"] = "2.3522";

    bool saveResult = dbManager->saveEntry(testEntry);
    QCOMPARE(saveResult, true);

    QJsonArray positions = dbManager->getCurrentPositions();
    QCOMPARE(positions.size(), 1);
    QCOMPARE(positions[0].toObject()["name"].toString(), QString("TEST-1"));
}

void TestDatabaseManager::testHistoryAndPositionLogic() {
    dbManager->connect(testHost, testUser, testPass, testDb);

    // On simule un ballon qui se déplace (3 trames pour le même indicatif)
    for (int i = 1; i <= 3; ++i) {
        QJsonObject entry;
        entry["name"] = "BALLON-1";
        entry["lat"] = QString::number(48.0 + i); // 49, 50, 51
        entry["lng"] = "2.0";
        dbManager->saveEntry(entry);
    }

    // 1. Vérification de POSITION (ne doit contenir qu'UNE seule ligne, la dernière)
    QJsonArray positions = dbManager->getCurrentPositions();
    QCOMPARE(positions.size(), 1);
    QCOMPARE(positions[0].toObject()["lat"].toDouble(), 51.0); // Vérifie que c'est bien la dernière maj

    // 2. Vérification de HISTORIQUE (doit contenir 3 lignes)
    QSqlQuery query("SELECT COUNT(*) FROM HISTORIQUE WHERE name = 'BALLON-1'");
    query.next();
    QCOMPARE(query.value(0).toInt(), 3);
}

void TestDatabaseManager::testIncompleteData() {
    dbManager->connect(testHost, testUser, testPass, testDb);

    QJsonObject incompleteEntry;
    incompleteEntry["name"] = "FANTOME";
    // On ne met pas de lat/lng intentionnellement

    bool result = dbManager->saveEntry(incompleteEntry);
    QCOMPARE(result, true); // La DB accepte si les champs ne sont pas NOT NULL stricts

    QJsonArray positions = dbManager->getCurrentPositions();
    QCOMPARE(positions[0].toObject()["lat"].toDouble(), 0.0); // qt convertira le vide en 0.0
}

void TestDatabaseManager::testSqlInjectionPrevention() {
    dbManager->connect(testHost, testUser, testPass, testDb);

    QJsonObject maliciousEntry;
    // Tentative d'effacer la table via le nom
    maliciousEntry["name"] = "HACKER'; DROP TABLE POSITION;--";
    maliciousEntry["lat"] = "10.0";
    maliciousEntry["lng"] = "10.0";

    bool result = dbManager->saveEntry(maliciousEntry);
    QCOMPARE(result, true);

    // Si l'injection avait marché, la table n'existerait plus et ceci crasherait/retournerait 0
    QJsonArray positions = dbManager->getCurrentPositions();
    QVERIFY(positions.size() > 0);
    QCOMPARE(positions[0].toObject()["name"].toString(), QString("HACKER'; DROP TABLE POSITION;--"));
}

void TestDatabaseManager::testDisconnect() {
    dbManager->disconnect();
    QCOMPARE(dbManager->isConnected(), false);
}

QTEST_MAIN(TestDatabaseManager)
#include "tst_main.moc"
