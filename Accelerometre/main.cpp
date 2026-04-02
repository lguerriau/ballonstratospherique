/* * File:   main.cpp
 * Projet: Nacelle Alpha - Transmission UART vers ESP32 LoRa
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <wiringPi.h>
#include <wiringSerial.h> // Indispensable pour la communication UART
#include "MPU6050.h"

using namespace std;

// --- FLAGS DE VOL ---
volatile bool flag_BURST = false;
volatile bool flag_LANDING = false;
bool simulationMode = false;

// Variables pour ne pas spammer le port série avec le même statut
string dernierStatutEnvoye = "";

void callback_ZM(void);
void callback_FF(void);

int main(int argc, char** argv) {
    int fd_serial;

    try {
        cout << "--- Nacelle Alpha : Démarrage ---" << endl;

        // --- INITIALISATION DU PORT SÉRIE (UART) ---
        // /dev/serial0 est le port série par défaut sur Raspberry Pi
        // 115200 est la vitesse (Baudrate), elle doit être identique sur ton ESP32 !
        if ((fd_serial = serialOpen("/dev/serial0", 115200)) < 0) {
            throw std::runtime_error("Impossible d'ouvrir le port série UART.");
        }
        cout << "Port série UART ouvert avec succès (115200 bauds)." << endl;

        // --- INITIALISATION DU CAPTEUR ---
        try {
            mpu.begin(0x69); //
            simulationMode = false;
            mpu.setDLPFMode(MPU6050::DLPF_5); //
            mpu.onFreeFall(callback_FF);      //
            mpu.onZeroMotion(callback_ZM);    //
            mpu.enableFreeFall(0x80, 1);      //
            mpu.enableZeroMotion(0x05, 0xFF); 
        } catch (const runtime_error &e) {    //
            cout << "Capteur absent, passage en mode SIMULATION." << endl;
            simulationMode = true;
        }

        // --- BOUCLE PRINCIPALE ---
        while (1) {
            float az, temp;

            // 1. Acquisition
            if (simulationMode) {
                az = 1.0; 
                temp = 18.5;
            } else {
                az = mpu.getAccelZ();        //
                temp = mpu.getTemperature(); //
            }

            // 2. Détermination du statut actuel
            string statutActuel = "ASCENSION";
            if (flag_LANDING) statutActuel = "LANDING";
            else if (flag_BURST) statutActuel = "BURST";

            // 3. Envoi des Télémétries par UART vers l'ESP32
            // L'ESP32 lira ces lignes avec Serial.readStringUntil('\n')
            serialPrintf(fd_serial, "TEMP:%.2f\n", temp);
            serialPrintf(fd_serial, "ACCELZ:%.2f\n", az);

            // 4. Envoi du Flag de vol (Uniquement s'il change pour économiser la bande passante LoRa)
            if (statutActuel != dernierStatutEnvoye) {
                serialPrintf(fd_serial, "STATUS:%s\n", statutActuel.c_str());
                cout << ">>> NOUVEAU STATUT TRANSMIS : " << statutActuel << " <<<" << endl;
                dernierStatutEnvoye = statutActuel;
            }

            cout << "Données envoyées à l'ESP32 -> Temp: " << temp << "°C | AccelZ: " << az << "g | Statut: " << statutActuel << endl;

            // Pause de 1 seconde entre chaque trame (Standard APRS/LoRa)
            sleep(1); //
        }

    } catch (const exception &e) {
        cout << "Erreur FATALE : " << e.what() << endl;
    }
    
    serialClose(fd_serial);
    return 0;
}

// --- INTERRUPTIONS DU CAPTEUR ---
void callback_ZM(void) { flag_LANDING = true; flag_BURST = false; }
void callback_FF(void) { if (!flag_LANDING) flag_BURST = true; }