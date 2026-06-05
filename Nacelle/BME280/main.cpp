/**
 * @file main.cpp
 * @brief Point d'entrée de la station météo - Lecture du BME280 et stockage JSON.
 */

#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>   
#include <iomanip>   
#include <cmath>

#include "bme280.h"

using namespace std;

/**
 * @brief Génère un horodatage textuel précis basé sur l'heure système.
 * @return Chaîne de caractères au format "AAAA-MM-JJ HH:MM:SS".
 */
string getTimestamp() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

/**
 * @brief Boucle principale du programme.
 * * Initialise le capteur sur le bus I2C local, effectue une lecture séquentielle
 * toutes les 10 secondes et stocke les données formatées en JSON en vue d'une
 * future intégration RadioLib / LoRa.
 * * @return 0 en cas de fonctionnement normal, -1 en cas d'erreur matérielle.
 */
int main() {
    cout << "[SYSTEME] Initialisation du module Capteur BME280..." << endl;

    try {
        BME280 capteur(0x77);
        string nomFichier = "mesures.json";

        cout << "Capteur BME280 détecté sur 0x77. Lecture en cours..." << endl;
        
        while (true) {
            float t_c = capteur.obtenirTemperatureEnC();
            float p   = capteur.obtenirPression();
            float h   = capteur.obtenirHumidite();

            cout << "[" << getTimestamp() << "] "
                 << "Temp: " << fixed << setprecision(2) << t_c << "°C | "
                 << "Pression: " << p << " hPa | "
                 << "Hum: " << h << "%" << endl;

            // --- SAUVEGARDE JSON LOCALE ---
            ofstream fichier(nomFichier, ios::app);
            if (fichier.is_open()) {
                fichier << "{\"date\":\"" << getTimestamp() 
                        << "\", \"t\":" << t_c 
                        << ", \"p\":" << p 
                        << ", \"h\":" << h << "}" << endl;
                fichier.close();
            }

            // Intervalle temporaire avant envoi radio via RadioLib
            this_thread::sleep_for(chrono::seconds(10));
        }
    } catch (const exception &e) {
        cerr << "Erreur critique Capteur : " << e.what() << endl;
        return -1;
    }

    return 0;
}