/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.h to edit this template
 */

/* 
 * File:   bdd.h
 * Author: hkalo
 *
 * Created on 3 avril 2026, 10:29
 */

#ifndef BDD_H
#define BDD_H

#include <mariadb/mysql.h>
#include <string>
#include <map>


class BDD {
public:
    
    BDD();
    ~BDD();
    
    bool connecter();
    void deconnecter();
    
    bool enregistrerPhotoBDD(std::string cheminFichier);
    
private:
    MYSQL *conn;
    std::map<std::string, std::string> chargerConfig(std::string nomFichier);

};



#endif /* CAM_H */
