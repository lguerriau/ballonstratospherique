#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFile>
#include <QTextStream>
#include <QtTest>
#include <QSignalSpy>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>

#include "databasemanager.h"

// ==========================================================
// 1. LA CLASSE DE TEST
// ==========================================================
class TestDatabaseManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() { db = new DatabaseManager(this); }

    void cleanup() {
        if (db->isConnected()) {
            QSqlQuery q;
            q.exec("TRUNCATE TABLE POSITION");
            q.exec("TRUNCATE TABLE HISTORIQUE");
        }
    }

    void test1_1_InitialState() { QCOMPARE(db->isConnected(), false); }

    void test1_2_ConnectionInvalidIP() {
        QSignalSpy spy(db, &DatabaseManager::errorOccurred);
        bool ok = db->connect("0.0.0.0", "root", "toto", "ballon2026_test");
        QCOMPARE(ok, false);
        QVERIFY(spy.count() > 0);
    }

    void test1_3_ConnectionInvalidCreds() {
        bool ok = db->connect("172.18.58.85", "mauvais", "mauvais", "ballon2026_test");
        QCOMPARE(ok, false);
    }

    void test1_4_ConnectionInvalidDB() {
        bool ok = db->connect("172.18.58.85", "root", "toto", "mauvaise_base");
        QCOMPARE(ok, false);
    }

    void test1_5_ConnectionValid() {
        bool ok = db->connect("172.18.58.85", "root", "toto", "ballon2026_test");
        QCOMPARE(ok, true);
        QCOMPARE(db->isConnected(), true);
    }

    void test1_6_DisconnectPropre() {
        db->connect("172.18.58.85", "root", "toto", "ballon2026_test");
        db->disconnect();
        QCOMPARE(db->isConnected(), false);
    }

    void test1_7_DisconnectRedondant() {
        db->disconnect();
        db->disconnect();
        QCOMPARE(db->isConnected(), false);
    }

    void test2_1_ReadOffline() {
        db->disconnect();
        QJsonArray res = db->getCurrentPositions();
        QCOMPARE(res.size(), 0);
    }

    void test2_2_WriteOffline() {
        db->disconnect();
        QJsonObject obj; obj["name"] = "TEST";
        bool ok = db->saveEntry(obj);
        QCOMPARE(ok, false);
    }

    void test2_3_EmptyJson() {
        db->connect("172.18.58.85", "root", "toto", "ballon2026_test");
        QJsonObject empty;
        bool ok = db->saveEntry(empty);
        QCOMPARE(ok, true);
    }

    void test2_4_IncompleteData() {
        db->connect("172.18.58.85", "root", "toto", "ballon2026_test");
        QJsonObject incomplete; incomplete["name"] = "PARTIEL";
        QVERIFY(db->saveEntry(incomplete));
    }

    void test2_5_SqlInjection() {
        db->connect("172.18.58.85", "root", "toto", "ballon2026_test");
        QJsonObject hack; hack["name"] = "HACKER'; DROP TABLE POSITION;--";
        QVERIFY(db->saveEntry(hack));
        QSqlQuery q("SELECT COUNT(*) FROM POSITION");
        QVERIFY(q.exec());
    }

    void test3_1_ReadEmptyDB() {
        db->connect("172.18.58.85", "root", "toto", "ballon2026_test");
        cleanup();
        QCOMPARE(db->getCurrentPositions().size(), 0);
    }

    void test3_2_FirstInsertion() {
        db->connect("172.18.58.85", "root", "toto", "ballon2026_test");
        QJsonObject entry; entry["name"] = "BALLON-1"; entry["lat"] = "48.5"; entry["lng"] = "2.3";
        QVERIFY(db->saveEntry(entry));
    }

    void test3_3_ReadAfterInsertion() {
        db->connect("172.18.58.85", "root", "toto", "ballon2026_test");
        QJsonObject entry; entry["name"] = "BALLON-1"; entry["lat"] = "48.5"; entry["lng"] = "2.3";
        db->saveEntry(entry);
        QJsonArray res = db->getCurrentPositions();
        QCOMPARE(res.size(), 1);
        QCOMPARE(res[0].toObject()["name"].toString(), QString("BALLON-1"));
    }

    void test3_4_UpdateAndHistoryLogic() {
        db->connect("172.18.58.85", "root", "toto", "ballon2026_test");
        QJsonObject move; move["name"] = "BALLON-1"; move["lat"] = "49.1";
        db->saveEntry(move);
        QSqlQuery qPos("SELECT lat FROM POSITION WHERE name = 'BALLON-1'");
        if(qPos.next()) QCOMPARE(qPos.value(0).toDouble(), 49.1);
    }

    void test3_5_MultiTargets() {
        db->connect("172.18.58.85", "root", "toto", "ballon2026_test");

        QJsonObject target1; target1["name"] = "BALLON-1"; target1["lat"] = "48.0";
        db->saveEntry(target1);

        QJsonObject target2; target2["name"] = "AVION-99"; target2["lat"] = "45.0";
        db->saveEntry(target2);

        QSqlQuery q("SELECT COUNT(*) FROM POSITION");
        if(q.next()) QCOMPARE(q.value(0).toInt(), 2);
    }

    void test3_6_ReadMultiTargets() {
        db->connect("172.18.58.85", "root", "toto", "ballon2026_test");

        QJsonObject target1; target1["name"] = "BALLON-1"; target1["lat"] = "48.0";
        db->saveEntry(target1);

        QJsonObject target2; target2["name"] = "AVION-99"; target2["lat"] = "45.0";
        db->saveEntry(target2);

        QJsonArray res = db->getCurrentPositions();
        QCOMPARE(res.size(), 2);
    }

private:
    DatabaseManager *db;
};

// ==========================================================
// 2. L'INTERFACE GRAPHIQUE DU TESTEUR
// ==========================================================
class TestRunnerUI : public QWidget
{
    Q_OBJECT

public:
    TestRunnerUI(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Tableau de bord - Tests Unitaires");
        resize(1000, 600);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // --- Création de la grille de boutons ---
        QGridLayout *gridLayout = new QGridLayout();

        QStringList testNames = {
            "test1_1_InitialState", "test1_2_ConnectionInvalidIP", "test1_3_ConnectionInvalidCreds",
            "test1_4_ConnectionInvalidDB", "test1_5_ConnectionValid", "test1_6_DisconnectPropre",
            "test1_7_DisconnectRedondant", "test2_1_ReadOffline", "test2_2_WriteOffline",
            "test2_3_EmptyJson", "test2_4_IncompleteData", "test2_5_SqlInjection",
            "test3_1_ReadEmptyDB", "test3_2_FirstInsertion", "test3_3_ReadAfterInsertion",
            "test3_4_UpdateAndHistoryLogic", "test3_5_MultiTargets", "test3_6_ReadMultiTargets"
        };

        int row = 0;
        int col = 0;
        for (const QString &testName : testNames) {
            QPushButton *btn = new QPushButton(testName, this);
            btn->setStyleSheet("padding: 5px; background-color: #2196F3; color: white; border-radius: 3px;");
            gridLayout->addWidget(btn, row, col);

            // On connecte le bouton à la fonction avec le nom du test spécifique
            connect(btn, &QPushButton::clicked, this, [this, testName]() {
                runTests(testName);
            });

            // On passe à la colonne suivante (3 colonnes max)
            col++;
            if (col > 2) {
                col = 0;
                row++;
            }
        }

        // --- Bouton "Lancer Tout" ---
        QPushButton *btnRunAll = new QPushButton("LANCER TOUS LES TESTS", this);
        btnRunAll->setStyleSheet("font-weight: bold; padding: 10px; background-color: #4CAF50; color: white;");
        connect(btnRunAll, &QPushButton::clicked, this, [this]() {
            runTests(""); // Une chaîne vide lance tous les tests
        });

        // --- Console de sortie ---
        txtOutput = new QTextEdit(this);
        txtOutput->setReadOnly(true);
        txtOutput->setStyleSheet("background-color: #1e1e1e; color: #00ff00; font-family: monospace; font-size: 13px;");

        // Ajout au layout principal
        mainLayout->addLayout(gridLayout);
        mainLayout->addWidget(btnRunAll);
        mainLayout->addWidget(txtOutput);
    }

private:
    QTextEdit *txtOutput;

    // Fonction commune pour lancer un ou tous les tests
    void runTests(const QString &testName) {
        txtOutput->clear();

        if (testName.isEmpty()) {
            txtOutput->append("[INFO] Demarrage de la suite complete des tests...\n");
        } else {
            txtOutput->append("[INFO] Execution du test individuel : " + testName + "\n");
        }
        QCoreApplication::processEvents();

        TestDatabaseManager testObj;
        QStringList args;
        args << "TestDatabaseManager";

        // Si on a cliqué sur un test spécifique, on l'ajoute aux arguments
        if (!testName.isEmpty()) {
            args << testName;
        }

        args << "-o" << "test_results.txt";

        // Exécution silencieuse
        QTest::qExec(&testObj, args);

        // Lecture et affichage des résultats
        QFile file("test_results.txt");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            txtOutput->append(in.readAll());
            file.close();
        } else {
            txtOutput->append("[ERREUR] Impossible de lire le fichier de resultats.");
        }

        txtOutput->append("\n[INFO] Termine.");
    }
};

// ==========================================================
// 3. LE MAIN
// ==========================================================
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TestRunnerUI w;
    w.show();
    return a.exec();
}

#include "tst_main.moc"
