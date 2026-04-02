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
#include <iostream>

int main() {
    std::cout << "--- Lancement du Test Unitaire Caméra ---" << std::endl;

    // Initialisation : Fréquence 29MHz et ton indicatif
    Camera maCam(29000000, "TEST-SSTV");

    // Test 1 : La Mire (Vérifie ImageMagick + dossier /home/pbs/sstv/)
    std::cout << "1. Génération et envoi de la mire..." << std::endl;
    maCam.envoyerMire();

    // Test 2 : Photo SSTV (Vérifie rpicam-still + conversion RGB + pisstv)
    std::cout << "2. Prise de photo et envoi SSTV (Basse Résolution)..." << std::endl;
    maCam.envoyerPhoto();

    // Test 3 : Photo HD (Vérifie le dossier /home/pbs/photos/)
    std::cout << "3. Enregistrement photo HD..." << std::endl;
    maCam.enregistrerPhoto();

    std::cout << "--- Test terminé avec succès ---" << std::endl;
    return 0;
}