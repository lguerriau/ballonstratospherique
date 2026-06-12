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
#include <cassert>
#include <QDebug>

/**
 * @brief Bdd::Bdd
 * @param parent pointeur vers le widget parent, par défaut nullptr
 * @author Harold KALO
 */
Bdd::Bdd(QWidget *parent) : QWidget(parent)
{
    this->db = QSqlDatabase::addDatabase("QMARIADB");

    // Règle 5 : Vérification que le driver SQL est correctement instancié
    assert(this->db.isValid());

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
    QSettings settings("config.ini", QSettings::IniFormat);

    // Règle 7 & 5 : Validation que le fichier de config est lisible ou possède des valeurs par défaut valides
    this->host     = settings.value("BDD/host", "172.18.58.85").toString();
    this->user     = settings.value("BDD/user", "root").toString();
    this->password = settings.value("BDD/password", "toto").toString();
    this->dbName   = settings.value("BDD/dbname", "ballon2026").toString();

    assert(!this->host.isEmpty());
    assert(!this->dbName.isEmpty());

    qDebug() << "Configuration chargée pour l'hôte :" << this->host;
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

    // Règle 7 : Contrôle strict de l'état de retour de la connexion réseau
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
    // Règle 5 : Post-condition assurant la fermeture
    assert(!this->db.isOpen());
}

/**
 * @brief Bdd::enregistrerPhoto
 * @param cheminFichier référence constante vers la chaîne contenant le chemin absolu du fichier image
 * @param pourWeb booléen déterminant si la photo doit être immédiatement partagée sur le site internet
 * @return bool true si la transaction INSERT s'est finalisée avec succès, false sinon
 * @author Harold KALO
 */
bool Bdd::enregistrerPhoto(const QString &cheminFichier, bool pourWeb) {
    // Règle 5 : Validation des paramètres d'entrée
    assert(!cheminFichier.isEmpty());

    if (!this->db.isOpen()) {
        if (!this->connecter()) {
            return false;
        }
    }

    // Règle 5 : Deuxième assertion pour garantir l'état du canal SQL avant écriture
    assert(this->db.isOpen());

    QSqlQuery query(this->db);
    bool p_ok = query.prepare("INSERT INTO IMAGES (chemin_image, horodatage_image, disponible_web) VALUES (:path, NOW(), :web)");
    assert(p_ok);

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
