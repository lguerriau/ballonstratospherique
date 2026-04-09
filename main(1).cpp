#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>   
#include <cmath>

// On inclut l'en-tête du capteur (depuis ton autre projet)
#include "../BME280/bme280.h"

// ==========================================
// --- AJOUT MPU6050 : INCLUSION ---
#include "MPU6050.h"
// ==========================================

// Bibliothèque RadioLib pour la partie LoRa
#include "RadioLib.h"
#include "RadioLib/src/hal/RPi/PiHal.h"

using namespace std;

// ==========================================
// --- AJOUT MPU6050 : LOGIQUE EN ARRIERE-PLAN ---
enum EtatVol { AU_SOL, ASCENSION, BURST, LANDING };
volatile EtatVol etatActuel = AU_SOL;
bool simulationModeMPU = false;

// Ce thread surveille le MPU6050 en continu sans bloquer le reste du programme
void surveillanceMPU() {
    try {
        mpu.begin(0x69);
        mpu.calibrateA();
        mpu.setDLPFMode(MPU6050::DLPF_5);
    } catch (...) {
        simulationModeMPU = true; // Tolérance aux pannes si le capteur est débranché
    }
    
    while (true) {
        float az = simulationModeMPU ? 1.00 : mpu.getAccelZ();
        
        if (etatActuel == AU_SOL) {
            if (az > 1.20f || az < 0.80f) etatActuel = ASCENSION;
        } else if (etatActuel == ASCENSION) {
            if (az < 0.40f) etatActuel = BURST;
        } else if (etatActuel == BURST) {
            if (az > 0.85f && az < 1.15f) etatActuel = LANDING;
        }
        // Analyse 2 fois par seconde
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}
// ==========================================


// --- CONFIGURATION RADIO ---
const string INDICATIF = "F4KMN-9"; 
const float FREQUENCE  = 433.775; // Fréquence APRS LoRa
const int SF = 12;                
const float BW = 125.0;           

// Initialisation du matériel (Bus SPI 0)
PiHal* hal = new PiHal(0); 

// --- CONFIGURATION DES PINS (Numérotation BCM) ---
// Utilisation des pins BCM pour éviter le conflit "Busy" sur la Pin 7 :
// NSS: 8, DIO0: 4 (Correspond à la Pin physique 7), RST: 17, DIO1: 18
SX1278 lora = new Module(hal, RADIOLIB_NC, 4, 17, 18);

// Fonction pour l'horodatage APRS (Format: MMDDhhmm en UTC)
string getAPRSTimestamp() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm* gmt = gmtime(&t);
    stringstream ss;
    ss << setfill('0') << setw(2) << gmt->tm_mon + 1
       << setw(2) << gmt->tm_mday
       << setw(2) << gmt->tm_hour
       << setw(2) << gmt->tm_min;
    return ss.str();
}

int main() {
    cout << "[SYSTEME] Lancement du module LoRa indépendant..." << endl;

    // ==========================================
    // --- AJOUT MPU6050 : LANCEMENT DU THREAD ---
    std::thread mpuThread(surveillanceMPU);
    mpuThread.detach(); // Permet au thread de vivre sa vie en arrière-plan
    // ==========================================

    // 1. Initialisation de la radio SX1278
    int state = lora.begin(FREQUENCE, BW, SF);
    if (state != RADIOLIB_ERR_NONE) {
        cout << "Erreur critique LoRa : " << state << endl;
        return -1;
    }
    
    try {
        // 2. Connexion au capteur BME280 via l'adresse I2C 0x77
        // La classe BME280 vient de ton projet séparé
        BME280 capteur(0x77); 
        
        cout << "Capteur distant (0x77) et RadioLib connectés." << endl;

        while (true) {
            // Lecture des données via l'interface du projet BME280
            float tempF = capteur.obtenirTemperatureEnF();
            float hum   = capteur.obtenirHumidite();
            float press = capteur.obtenirPression();

            // --- CONSTRUCTION DE LA TRAME APRS ---
            stringstream aprs;
            aprs << INDICATIF << ">APRS,WIDE1-1:_" << getAPRSTimestamp();
            aprs << "c...s...g..."; // Pas d'anémomètre
            
            // Température en Fahrenheit (3 chiffres)
            aprs << "t" << setfill('0') << setw(3) << (int)round(tempF);
            
            // Humidité (2 chiffres)
            int h_int = (int)round(hum);
            aprs << "h" << setfill('0') << setw(2) << (h_int >= 100 ? 0 : h_int);
            
            // Pression (5 chiffres, dixièmes de hPa)
            aprs << "b" << setfill('0') << setw(5) << (int)round(press * 10.0);

            // ==========================================
            // --- AJOUT MPU6050 : INTEGRATION DU FLAG ---
            // On convertit l'état interne en texte compatible avec l'interface Qt
            string statutStr = "en vol"; // Par défaut au sol ou en ascension
            if (etatActuel == BURST) statutStr = "BURST";
            else if (etatActuel == LANDING) statutStr = "LANDING";
            
            // Ajout du flag à la fin de la trame météo, précédé de " :ST:"
            // Ainsi la fonction lastIndexOf(':') de la Gateway le trouvera direct !
            aprs << " :ST:" << statutStr;
            // ==========================================

            string trame = aprs.str();
            cout << "Transmission : " << trame << endl;

            // --- ENVOI RADIO ---
            state = lora.transmit(trame.c_str());

            if (state == RADIOLIB_ERR_NONE) {
                cout << "-> Succès !" << endl;
            } else {
                cout << "-> Échec (Code : " << state << ")" << endl;
            }

            // Pause de 30 secondes
            this_thread::sleep_for(chrono::seconds(30));
        }
    } catch (const exception &e) {
        cerr << "Erreur : " << e.what() << endl;
    }

    return 0;
}
