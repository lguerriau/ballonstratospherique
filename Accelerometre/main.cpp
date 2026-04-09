/* * File:   main.cpp - MPU6050 v5 (Logique 100% logicielle sur Axe Z) */

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>
#include <cmath> // Pour la valeur absolue abs()
#include "MPU6050.h"

using namespace std;

// --- LES 4 ÉTATS DU VOL ---
enum EtatVol { AU_SOL, ASCENSION, BURST, LANDING };
EtatVol etatActuel = AU_SOL; 
bool simulationMode = false;

int main(int argc, char** argv) {
    cout << "--- Nacelle Alpha : Service MPU6050 (Logique Axe Z) ---" << endl;

    try {
        mpu.begin(0x69);
        simulationMode = false;
        
        cout << "Calibration en cours... NE PAS BOUGER LA NACELLE" << endl;
        mpu.calibrateA(); // Remet le capteur parfaitement à 1.00g au repos
        
        mpu.setDLPFMode(MPU6050::DLPF_5); // On garde juste le filtre anti-vibrations
        
        // ATTENTION : On a supprimé toutes les interruptions (callbacks, enableMotion, etc.)
        // C'est maintenant la boucle principale qui fait tout le travail !
        
        cout << "Capteur prêt. Analyse logicielle de l'axe Z activée." << endl;
    } catch (const runtime_error &e) {
        cout << "Capteur absent, mode SIMULATION." << endl;
        simulationMode = true;
    }

    while (true) {
        // 1. LECTURE DE L'AXE Z
        float az = simulationMode ? 1.00 : mpu.getAccelZ();

        // 2. MACHINE À ÉTATS LOGICIELLE (L'intelligence de la nacelle)
        if (etatActuel == AU_SOL) {
            // Si on s'écarte de plus de 0.2g de la gravité normale (1.00g), ça bouge !
            if (az > 1.20 || az < 0.80) {
                etatActuel = ASCENSION;
            }
        } 
        else if (etatActuel == ASCENSION) {
            // Si on approche de 0g (chute libre / apesanteur)
            if (az < 0.40) {
                etatActuel = BURST;
            }
        } 
        else if (etatActuel == BURST) {
            // Si la gravité revient à la normale (entre 0.8 et 1.2g) après la chute
            if (az > 0.80 && az < 1.20) {
                etatActuel = LANDING;
            }
        }

        // 3. PRÉPARATION DE L'EXPORTATION
        string statutStr;
        switch(etatActuel) {
            case AU_SOL:    statutStr = "AU SOL";  break;
            case ASCENSION: statutStr = "en vol";  break;
            case BURST:     statutStr = "BURST";   break;
            case LANDING:   statutStr = "LANDING"; break;
        }

        // 4. ÉCRITURE DANS LES FICHIERS POUR LORA
        ofstream fStat("/tmp/mpu6050_status.txt", ios::trunc);
        if (fStat.is_open()) { fStat << statutStr; fStat.close(); }

        ofstream fAccel("/tmp/mpu6050_accel_z.txt", ios::trunc);
        if (fAccel.is_open()) { fAccel << fixed << setprecision(2) << az; fAccel.close(); }

        // 5. AFFICHAGE CONSOLE
        cout <<"\r"<< "[LOG] Statut: " << setw(8) << statutStr << " | Accel Z: " << fixed << setprecision(2) << az << " g" << endl;

        // Boucle plus rapide (500ms) pour ne pas rater une chute rapide de ta main
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    return 0;
}