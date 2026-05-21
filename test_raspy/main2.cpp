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
#include <cassert>

int main() {
    std::cout << "=== LANCEMENT NACELLE RASPBERRY ===" << std::endl;

    Camera maCam(29000000, "F4KMN");
    int compteurSSTV = 0;

    // Règle 2: Estimation du temps max de vol du ballon (ex: 4 heures = 14400 secondes).
    // Une boucle toutes les 30s => Max 480 itérations. On borne à 1000 itérations max.
    const unsigned int MAX_CYCLES_VOL = 1000; 

    // Règle 1 & 2: Plus de while(true), boucle strictement bornée et vérifiable statiquement
    for (unsigned int cycle = 0; cycle < MAX_CYCLES_VOL; ++cycle) {
        
        // Règle 5: Deux assertions de contrôle d'état
        assert(compteurSSTV >= 0);
        assert(compteurSSTV <= 10);

        maCam.enregistrerPhoto();
        compteurSSTV++;

        if (compteurSSTV >= 10) {
            std::cout << "--- Cycle Radio (5 min) ---" << std::endl;
            maCam.envoyerPhoto();
            compteurSSTV = 0; 
        }

        std::cout << "Attente 30s (Cycle " << cycle << "/" << MAX_CYCLES_VOL << ")..." << std::endl;
        sleep(30); 
    }

    // Règle 5: L'assertion finale pour prouver la sortie normale
    assert(compteurSSTV < 10);

    return 0;
}