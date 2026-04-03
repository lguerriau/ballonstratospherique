/* * File:   main.cpp
 * Projet: Nacelle Alpha - Transmission UART optimisée LoRa
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "MPU6050.h"

using namespace std;

// --- FLAGS DE VOL ---
volatile bool flag_BURST = false;
volatile bool flag_LANDING = false;
bool simulationMode = false;

void callback_ZM(void);
void callback_FF(void);

int main(int argc, char** argv) {
    int fd_serial;

    try {
        cout << "--- Nacelle Alpha : Démarrage (Mode Optimisé) ---" << endl;

        // --- INITIALISATION DU PORT SÉRIE (UART) ---
        if ((fd_serial = serialOpen("/dev/serial0", 115200)) < 0) {
            throw std::runtime_error("Impossible d'ouvrir le port série UART.");
        }
        cout << "Port série UART ouvert (115200 bauds)." << endl;

        // --- INITIALISATION DU CAPTEUR ---
        try {
            mpu.begin(0x69); //
            simulationMode = false;
            mpu.setDLPFMode(MPU6050::DLPF_5); //
            mpu.onFreeFall(callback_FF);      //
            mpu.onZeroMotion(callback_ZM);    //
            mpu.enableFreeFall(0x80, 1);      //
            mpu.enableZeroMotion(0x05, 0xFF); //
        } catch (const runtime_error &e) {
            cout << "Capteur absent, passage en mode SIMULATION." << endl;
            simulationMode = true;
        }

        // --- BOUCLE PRINCIPALE ---
        while (1) {
            float az;

            // 1. Acquisition (uniquement l'axe Z)
            if (simulationMode) {
                az = 1.0; 
            } else {
                az = mpu.getAccelZ(); //
            }

            // 2. Détermination du statut actuel
            string statutActuel = "en vol";
            if (flag_LANDING) statutActuel = "LANDING";
            else if (flag_BURST) statutActuel = "BURST";

            // 3. Envoi des Télémétries brutes par UART vers l'ESP32
            // Format ultra-court pour faciliter la création de la trame LoRa
            serialPrintf(fd_serial, "Z:%.2f\n", az);
            serialPrintf(fd_serial, "ST:%s\n", statutActuel.c_str());

            // Affichage console épuré pour le débogage local
            cout << "Z:" << az << " | ST:" << statutActuel << endl;

            // Pause de 1 seconde entre chaque cycle
            sleep(1); //
        }

    } catch (const exception &e) {
        cout << "Erreur FATALE : " << e.what() << endl; //
    }
    
    serialClose(fd_serial);
    return 0;
}

// --- INTERRUPTIONS DU CAPTEUR ---
void callback_ZM(void) { flag_LANDING = true; flag_BURST = false; } //
void callback_FF(void) { if (!flag_LANDING) flag_BURST = true; } //