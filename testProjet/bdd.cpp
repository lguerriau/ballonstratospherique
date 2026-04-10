/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */

#include "bdd.h"
#include <iostream>
#include <fstream>
#include <map>

BDD::BDD() {
    conn = mysql_init(NULL);
    if (conn == NULL) {
        std::cerr << "Erreur : Initialisation MySQL impossible." << std::endl;
    }
}

BDD::~BDD() {
    deconnecter();
}

std::map<std::string, std::string> BDD::chargerConfig(std::string nomFichier) {
    std::map<std::string, std::string> config;
    std::ifstream fichier(nomFichier);
    std::string ligne;

    if (fichier.is_open()) {
        while (std::getline(fichier, ligne)) {
            size_t pos = ligne.find('=');
            if (pos != std::string::npos) {
                // On sépare la clé (ex: host) de la valeur (ex: localhost)
                std::string cle = ligne.substr(0, pos);
                std::string valeur = ligne.substr(pos + 1);
                config[cle] = valeur;
            }
        }
        fichier.close();
    } else {
        std::cerr << "Erreur : Impossible de trouver le fichier " << nomFichier << std::endl;
    }
    return config;
}

    bool BDD::connecter() {
        // On récupère les infos du fichier config.ini
        // Note : Utilise le nom de fichier que tu as créé sur la Raspberry
        std::map<std::string, std::string> config = chargerConfig("config.ini");

        if (!mysql_real_connect(conn,
                config["host"].c_str(),
                config["username"].c_str(),
                config["password"].c_str(),
                config["database"].c_str(), 0, NULL, 0)) {

            std::cerr << "Erreur de connexion MariaDB : " << mysql_error(conn) << std::endl;
            return false;
        }                     

        std::cout << "Connexion reussie a la base : " << config["dbname"] << std::endl;
        return true;
    }

    bool BDD::enregistrerPhotoBDD(std::string cheminFichier) {
        // On prépare les deux requêtes SQL
        // On met le chemin entre des guillemets simples '' pour le SQL
        std::string req1 = "INSERT INTO IMAGE (chemin_image) VALUES ('" + cheminFichier + "');";
        std::string req2 = "INSERT INTO IMGINTERNET (chemin_imageint) VALUES ('" + cheminFichier + "');";

        // Exécution sur la table IMAGE
        if (mysql_query(conn, req1.c_str())) {
            std::cerr << "Erreur table IMAGE : " << mysql_error(conn) << std::endl;
            return false;
        }

        // Exécution sur la table IMGINTERNET
        if (mysql_query(conn, req2.c_str())) {
            std::cerr << "Erreur table IMGINTERNET : " << mysql_error(conn) << std::endl;
            return false;
        }

        return true;
    }

    void BDD::deconnecter() {
        if (conn) {
            mysql_close(conn);
            conn = NULL;
        }
    }

