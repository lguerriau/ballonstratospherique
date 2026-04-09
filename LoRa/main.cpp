#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>   
#include <cmath>

// On inclut l'en-tête du capteur
#include "../BME280/bme280.h"
#include "../LM75/LM75.h"

// Bibliothèque RadioLib pour la partie LoRa
#include "RadioLib.h"
#include "RadioLib/src/hal/RPi/PiHal.h"

using namespace std;

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

    // 1. Initialisation de la radio SX1278
    int state = lora.begin(FREQUENCE, BW, SF);
    if (state != RADIOLIB_ERR_NONE) {
        cout << "Erreur critique LoRa : " << state << endl;
        return -1;
    }
    
    try {
        // Initialisation des capteurs (BME280 pour humidité/pression, LM75 pour température)
        BME280 capteurBME(0x77); 
        LM75   capteurLM(0x48);   // Utilisation de la classe du projet LM75 à l'adresse 0x48

        cout << "[SYSTEME] Capteurs BME280 et LM75 connectés." << endl;

        while (true) {
            // Lecture des données du BME280
            float hum   = capteurBME.obtenirHumidite();
            float press = capteurBME.obtenirPression();

            // Utilisation de la méthode getTemperature() issue de ton projet LM75
            float tempC_LM = capteurLM.getTemperature();
            float tempF_LM = (tempC_LM * 1.8) + 32; // Conversion Celsius vers Fahrenheit pour le format APRS

            // --- CONSTRUCTION DE LA TRAME APRS ---
            stringstream aprs;
            aprs << INDICATIF << ">APRS,WIDE1-1:_" << getAPRSTimestamp();
            aprs << "c...s...g..."; // Champs vent non utilisés
            
            // Température du LM75 en Fahrenheit (format 3 chiffres)
            aprs << "t" << setfill('0') << setw(3) << (int)round(tempF_LM);
            
            // Humidité (format 2 chiffres)
            int h_int = (int)round(hum);
            aprs << "h" << setfill('0') << setw(2) << (h_int >= 100 ? 0 : h_int);
            
            // Pression (format 5 chiffres, dixièmes de hPa)
            aprs << "b" << setfill('0') << setw(5) << (int)round(press * 10.0);

            string trame = aprs.str();
            cout << "Transmission (Temp LM75: " << fixed << setprecision(2) << tempC_LM << "°C) : " << trame << endl;

            // --- ENVOI RADIO VIA RADIOLIB ---
            state = lora.transmit(trame.c_str());

            if (state == RADIOLIB_ERR_NONE) {
                cout << "-> Envoi réussi !" << endl;
            } else {
                cout << "-> Erreur d'envoi (Code : " << state << ")" << endl;
            }

            // Pause de 30 secondes entre chaque envoi
            this_thread::sleep_for(chrono::seconds(30));
        }
    } catch (const exception &e) {
        cerr << "Erreur critique : " << e.what() << endl;
    }

    return 0;
}