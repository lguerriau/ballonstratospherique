/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/main.cc to edit this template
 */

/* 
 * File:   main2.cpp
 * Author: hkalo
 *
 * Created on 2 avril 2026, 09:17
 */

#include "cam.h"
#include "bdd.h"
#include <iostream>

int main() {
    BDD maBdd;
    std::string cheminReel = "/home/pbs/photos/image.jpg";
    std::cout << "Tentative de connexion..." << std::endl;
    std::cout << "--- Lancement du Test Unitaire Caméra ---" << std::endl;
    system("pwd"); // Affiche le dossier de travail actuel dans la console

    // Initialisation : Fréquence 29MHz et ton indicatif
    Camera maCam(29000000, "TEST-SSTV");

    // Test 1 : La Mire (Vérifie ImageMagick + dossier /home/pbs/sstv/)
    std::cout << "1. Génération et envoi de la mire..." << std::endl;
    maCam.envoyerMire();

    // Test 2 : Photo SSTV (Vérifie rpicam-still + conversion RGB + pisstv)
    std::cout << "2. Prise de photo et envoi SSTV (Basse Résolution)..." << std::endl;
    maCam.envoyerPhoto();
    sleep(2);

    // Test 3 : Photo HD (Vérifie le dossier /home/pbs/photos/)
    std::cout << "3. Enregistrement photo HD..." << std::endl;
    maCam.enregistrerPhoto();
    
    std::cout << "--- Test terminé avec succès ---" << std::endl;
    
    if (maBdd.connecter()) {
        //maBdd.enregistrerPhotoBDD("/ramfs/test_connexion.jpg");
        std::cout << "Test reussi !" << std::endl;
        std::cout << "Enregistrement du chemin : " << cheminReel << std::endl;
        
        if (maBdd.enregistrerPhotoBDD(cheminReel)) {
            std::cout << "L'insertion a semble-t-il fonctionne !" << std::endl;
        } else {
            std::cout << "Erreur lors de l'insertion." << std::endl;
        }
        
        maBdd.deconnecter();
    } else {
        std::cout << "La connexion a echoue. Verifie ton config.ini et l'IP de la BDD." << std::endl;
    }
    std::cout << "--- Programme terminé ---" << std::endl;
    return 0;   
}