#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>   
#include <iomanip>   
#include <cmath>

// Inclusion du driver du capteur
#include "bme280.h"

using namespace std;

// Fonction pour l'horodatage JSON
string getTimestamp() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int main() {
    cout << "[SYSTEME] Initialisation du module Capteur BME280..." << endl;

    try {
        // Initialisation du capteur sur l'adresse 0x77
        BME280 capteur(0x77);
        string nomFichier = "mesures.json";

        cout << "Capteur BME280 détecté sur 0x77. Lecture en cours..." << endl;
        
        while (true) {
            // Lecture des données du capteur
            float t_c = capteur.obtenirTemperatureEnC();
            float p   = capteur.obtenirPression();
            float h   = capteur.obtenirHumidite();

            // Affichage console pour vérification
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

            // Attente de 10 secondes (plus rapide que le LoRa pour les tests)
            this_thread::sleep_for(chrono::seconds(10));
        }
    } catch (const exception &e) {
        cerr << "Erreur critique Capteur : " << e.what() << endl;
        return -1;
    }

    return 0;
}