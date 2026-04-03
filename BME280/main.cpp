#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>   
#include <iomanip>   
#include <cmath>

// Tes fichiers existants
#include "bme280.h"

// On simule ici l'interface avec un driver LoRa Linux (ex: RadioLib ou LoRa-Lib)
// Note : Tu devras installer une lib LoRa pour RPi (voir étape 3)
#include "RadioLib/src/RadioLib.h"
#include "RadioLib/src/hal/RPi/PiHal.h"
//#include "PiHal.h"

using namespace std;

// --- CONFIGURATION ---
const string INDICATIF = "F4KMN-9"; 
const float FREQUENCE  = 433.775; // Fréquence APRS LoRa (à adapter selon ta région)
const int SF = 12;                // Spreading Factor standard APRS
const float BW = 125.0;           // Bandwidth

// Instance du matériel pour RadioLib (Pinout RPi Zero)
PiHal* hal = new PiHal(0); // SPI bus 0
// NSS: 8 (CE0), DIO0: 7 (G25), RST: 0 (G17), DIO1: 1 (G18)
//SX1278 lora = new Module(hal, 8, 7, 0, 1);
SX1278 lora = new Module(hal, RADIOLIB_NC, 7, RADIOLIB_NC, RADIOLIB_NC);

// Fonction pour l'horodatage JSON
string getTimestamp() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

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
    // 1. Initialisation du LoRa
    cout << "[LoRa] Initialisation..." << endl;
    int state = lora.begin(FREQUENCE, BW, SF);
    if (state != RADIOLIB_ERR_NONE) {
        cout << "Erreur LoRa : " << state << endl;
        return -1;
    }

    try {
        // 2. Initialisation du Capteur
        BME280 capteur(0x77);
        string nomFichier = "mesures.json";

        cout << "Capteur BME 280 et LoRa PRÊTS." << endl;
        
        while (true) {
            // Lecture des données
            float t_c = capteur.obtenirTemperatureEnC();
            float p   = capteur.obtenirPression();
            float h   = capteur.obtenirHumidite();
            float t_f = capteur.obtenirTemperatureEnF(); // Dispo dans bme280.cpp

            // --- A. SAUVEGARDE JSON (Locale) ---
            ofstream fichier(nomFichier, ios::app);
            if (fichier.is_open()) {
                fichier << "{\"date\":\"" << getTimestamp() << "\", \"t\":" << t_c << ", \"p\":" << p << "}" << endl;
                fichier.close();
            }

            // --- B. CONSTRUCTION TRAME APRS WEATHER ---
            // Format : _MMDDhhmmc...s...g...tTTTrRRRpPPPhHHbPPPPP
            stringstream aprs;
            aprs << INDICATIF << ">APRS,WIDE1-1:_" << getAPRSTimestamp();
            aprs << "c...s...g..."; // On n'a pas d'anémomètre
            
            // Température en Fahrenheit (3 chiffres)
            aprs << "t" << setfill('0') << setw(3) << (int)round(t_f);
            
            // Humidité (2 chiffres, 00=100%)
            int hum = (int)round(h);
            aprs << "h" << setfill('0') << setw(2) << (hum >= 100 ? 0 : hum);
            
            // Pression (5 chiffres, dixièmes de hPa)
            aprs << "b" << setfill('0') << setw(5) << (int)round(p * 10.0);

            string trame = aprs.str();

            // --- C. ENVOI LORA ---
            cout << "Envoi trame : " << trame << endl;
           state = lora.transmit(trame.c_str());

            if (state == RADIOLIB_ERR_NONE) {
                cout << "Succès !" << endl;
            } else {
                cout << "Échec envoi, code : " << state << endl;
            }

            // Attente 1 minute (norme raisonnable pour la météo)
            this_thread::sleep_for(chrono::seconds(30));
        }
    } catch (const exception &e) {
        cerr << "Erreur fatale : " << e.what() << endl;
    }

    return 0;
}