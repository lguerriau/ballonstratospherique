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
#include <unistd.h>

int main() {
    std::cout << "=== LANCEMENT NACELLE RASPBERRY ===" << std::endl;

    // Fréquence radio 29MHz
    Camera maCam(29000000, "F4KMN");

    int compteurSSTV = 0;

    while(true) {
        // 1. On prend la photo HD toutes les 30 secondes
        maCam.enregistrerPhoto();
        compteurSSTV++;

        // 2. Toutes les 10 photos (soit environ 5 minutes), on envoie en radio
        if (compteurSSTV >= 10) {
            std::cout << "--- Cycle Radio (5 min) ---" << std::endl;
            maCam.envoyerPhoto();
            compteurSSTV = 0; // On reset le compteur
        }

        std::cout << "Attente 30s..." << std::endl;
        sleep(30); 
    }

    return 0; 
}