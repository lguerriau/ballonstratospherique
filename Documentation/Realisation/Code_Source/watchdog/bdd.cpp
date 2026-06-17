/**
   @file bdd.cpp
   @brief implemantation de la classe Bdd
   @version 1.0
   @author Harold KALO
   @date 26/05/2026
   @details Classe modélisant le remplissage de chemin photos dans la base de données faite pour enregistrer les photos
*/

#include "watchdog.h"
#include "ui_watchdog.h"
#include "bdd.h"
#include <QDebug>

/**
 * @brief Bdd::Bdd
 * @param parent pointeur vers le widget parent, par défaut nullptr
 * @author Harold KALO
 */
Bdd::Bdd(QWidget *parent) : QWidget(parent)
{
    this->db = QSqlDatabase::addDatabase("QMARIADB");

    if (!this->db.isValid()) {
        qDebug() << "[ERREUR CRITIQUE] : Le driver SQL (QMARIADB) n'est pas disponible ou invalide !";
        return;
    }

    this->chargerConfiguration();
}

/**
 * @brief Bdd::~Bdd
 * @author Harold KALO
 */
Bdd::~Bdd() {
    this->deconnecter();
}

/**
 * @brief Bdd::chargerConfiguration
 * @author Harold KALO
 */
void Bdd::chargerConfiguration() {
    QString pathConfig = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(pathConfig, QSettings::IniFormat);

    this->host     = settings.value("BDD/host", "127.0.0.1").toString();
    this->user     = settings.value("BDD/user", "root").toString();
    this->password = settings.value("BDD/password", "toto").toString();
    this->dbName   = settings.value("BDD/dbname", "ballon2026").toString();

    this->host.remove("\"");
    this->user.remove("\"");
    this->password.remove("\"");
    this->dbName.remove("\"");

    if (this->host.isEmpty()) {
        qDebug() << "[ATTENTION] : L'hôte de la BDD est vide dans config.ini. Utilisation de localhost par défaut.";
        this->host = "127.0.0.1";
    }
    if (this->dbName.isEmpty()) {
        qDebug() << "[ERREUR SYSTEME] : Le nom de la base de données est vide.";
    }

}

/**
 * @brief Bdd::connecter
 * @return bool true si la connexion réseau est établie, false en cas d'échec
 * @author Harold KALO
 */
bool Bdd::connecter() {
    this->db.setHostName(this->host);
    this->db.setUserName(this->user);
    this->db.setPassword(this->password);
    this->db.setDatabaseName(this->dbName);

    const bool action_ouverte = this->db.open();

    if (action_ouverte) {
        qDebug() << "Connexion réussie à MariaDB !";
    } else {
        qDebug() << "Échec de connexion :" << this->db.lastError().text();
    }
    return action_ouverte;
}


/**
 * @brief Bdd::deconnecter
 * @author Harold KALO
 */
void Bdd::deconnecter() {
    if (this->db.isOpen()) {
        this->db.close();
        qDebug() << "Déconnexion de la base de données.";
    }


    if (this->db.isOpen()) {
        qDebug() << "[ATTENTION] : La base de données ne s'est pas déconnectée correctement.";
    }
}

/**
 * @brief Bdd::enregistrerPhoto
 * @param cheminFichier référence constante vers la chaîne contenant le chemin absolu du fichier image
 * @param pourWeb booléen déterminant si la photo doit être immédiatement partagée sur le site internet
 * @return bool true si la transaction INSERT s'est finalisée avec succès, false sinon
 * @author Harold KALO
 */
bool Bdd::enregistrerPhoto(const QString &cheminFichier, bool pourWeb) {
    if (cheminFichier.isEmpty()) {
        qDebug() << "[ERREUR SQL] : Impossible d'enregistrer une photo sans nom de fichier.";
        return false;
    }

    if (!this->db.isOpen()) {
        if (!this->connecter()) {
            return false;
        }
    }

    if (!this->db.isOpen()) {
        qDebug() << "[ERREUR SQL] : Le canal SQL est fermé. Annulation de l'insertion.";
        return false;
    }

    QSqlQuery query(this->db);
    bool p_ok = query.prepare("INSERT INTO IMAGES (chemin_image, horodatage_image, disponible_web) VALUES (:path, NOW(), :web)");
    if (!p_ok) {
        qDebug() << "[ERREUR SQL] : Échec de la préparation de la requête INSERT :" << query.lastError().text();
        return false;
    }

    query.bindValue(":path", cheminFichier);
    query.bindValue(":web", pourWeb ? 1 : 0);

    const bool execution_ok = query.exec();
    if (execution_ok) {
        qDebug() << "Image enregistrée en BDD :" << cheminFichier;
    } else {
        qDebug() << "Erreur lors de l'insertion SQL :" << query.lastError().text();
    }

    return execution_ok;
}
