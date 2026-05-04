#include "widget.h"
#include "ui_widget.h"

Bdd::Bdd(QWidget *parent) : QWidget(parent)
{
    db = QSqlDatabase::addDatabase("QMARIADB");

    // On charge la configuration dès la création de l'objet
    chargerConfiguration();
}


void Bdd::chargerConfiguration() {
    // QSettings lit le fichier .ini dans le dossier d'exécution
    QSettings settings("config.ini", QSettings::IniFormat);

    // Lecture des clés : settings.value("CLE", "ValeurParDefaut")
    host     = settings.value("BDD/host", "172.18.58.85").toString();
    user     = settings.value("BDD/user", "root").toString();
    password = settings.value("BDD/password", "toto").toString();
    dbName   = settings.value("BDD/dbname", "ballon2026").toString();

    qDebug() << "Configuration chargée pour l'hôte :" << host;
}

bool Bdd::connecter() {
    // Application des paramètres à l'objet db
    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(password);
    db.setDatabaseName(dbName);

    if (db.open()) {
        qDebug() << "Connexion réussie à MariaDB !";
        return true;
    } else {
        qDebug() << "Échec de connexion :" << db.lastError().text();
        return false;
    }
}

void Bdd::deconnecter() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "Déconnexion de la base de données.";
    }
}

bool Bdd::enregistrerPhoto(QString cheminFichier, bool pourWeb) {
    if (!db.isOpen()) {
        if (!connecter()) return false;
    }

    QSqlQuery query;
    // On prépare la requête SQL
    query.prepare("INSERT INTO IMAGE (chemin_image, horodatage_image, disponible_web) VALUES (:path, NOW(), :web)");

    // On remplace le marqueur :path par le vrai chemin
    query.bindValue(":path", cheminFichier);
    query.bindValue(":web", pourWeb ? 1 : 0);

    if (query.exec()) {
        qDebug() << "Image enregistrée en BDD :" << cheminFichier;
        return true;
    } else {
        qDebug() << "Erreur lors de l'insertion SQL :" << query.lastError().text();
        return false;
    }
}
