/* * File:   main.cpp
 * Projet: Nacelle Alpha - MPU6050 (Exportation par Fichiers)
 */

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>

// Inclusion de ton fichier d'en-tête intact
#include "MPU6050.h"

using namespace std;

// --- FLAGS DE VOL ---
volatile bool flag_BURST = false;
volatile bool flag_LANDING = false;
bool simulationMode = false;

// Callbacks déclenchés par les interruptions matérielles du MPU6050
void callback_ZM() { flag_LANDING = true; flag_BURST = false; }
void callback_FF() { if (!flag_LANDING) flag_BURST = true; }

int main(int argc, char** argv) {
    cout << "--- Nacelle Alpha : Service Capteur MPU6050 ---" << endl;

    // 1. Initialisation de ton capteur MPU6050
    try {
        mpu.begin(0x69); // Appel à ta fonction begin
        simulationMode = false;
        
        // Configuration du filtre et des interruptions avec tes méthodes existantes
        mpu.setDLPFMode(MPU6050::DLPF_5);
        mpu.onFreeFall(callback_FF);
        mpu.onZeroMotion(callback_ZM);
        mpu.enableFreeFall(0x80, 1);
        mpu.enableZeroMotion(0x05, 0xFF);
        
        cout << "MPU6050 armé (0x69). Écriture des données dans /tmp/..." << endl;
    } catch (const runtime_error &e) {
        cout << "Capteur absent, passage en mode SIMULATION." << endl;
        simulationMode = true;
    }

    // 2. Boucle principale de lecture et d'exportation
    while (true) {
        // A. Détermination du statut de vol d'après tes drapeaux
        string statut = "en vol";
        if (flag_LANDING) statut = "LANDING";
        else if (flag_BURST) statut = "BURST";

        // B. Lecture de l'accélération (ou valeur fixe si simulation)
        float az = simulationMode ? 1.02 : mpu.getAccelZ();

        // --- EXPORTATION DES DONNÉES DANS DES FICHIERS ---
        // Le programme LoRa de ton camarade pourra lire ces fichiers à tout moment

        // Écriture du statut de vol
        std::ofstream fichierStatus("/tmp/mpu6050_status.txt", std::ios::trunc);
        if (fichierStatus.is_open()) {
            fichierStatus << statut;
            fichierStatus.close();
        } else {
            cerr << "Erreur : Impossible d'écrire le fichier status." << endl;
        }

        // Écriture de l'accélération Z
        std::ofstream fichierAccel("/tmp/mpu6050_accel_z.txt", std::ios::trunc);
        if (fichierAccel.is_open()) {
            fichierAccel << fixed << setprecision(2) << az;
            fichierAccel.close();
        }

        // --- AFFICHAGE CONSOLE (Pour le débugging) ---
        cout << "[MPU6050] Z: " << fixed << setprecision(2) << az << " g | Statut: " << statut << endl;

        // Rafraîchissement des données toutes les 1 seconde
        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}