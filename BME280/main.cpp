/* * File:   main.cpp
 * Author: philippe SIMIER
 *
 * Created on 27 mars 2024, 08:21
 * */

#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <fstream>   
#include <iomanip>   
#include "bme280.h"

using namespace std;

string getTimestamp() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int main(int argc, char** argv) {

    try {
        BME280 capteur(0x77);
        string nomFichier = "mesures.json"; // Le nom est fixé ici

        cout << "Capteur BME 280 présent sur le bus I2C\r" << endl;
        
        while (1) {
            float t = capteur.obtenirTemperatureEnC();
            float p = capteur.obtenirPression();
            float h = capteur.obtenirHumidite();
            string horodatage = getTimestamp();

            // Affichage inchangé
            cout << " Température (C)  : " << fixed << setprecision(1) << t << " °C\r" << endl;
            cout << " Pression         : " << fixed << setprecision(1) << p << " hPa\r" << endl;
            cout << " Humidité         : " << fixed << setprecision(1) << h << " %\r" << endl;

            // Écriture JSON
            ofstream fichier(nomFichier, ios::app);
            if (fichier.is_open()) {
                fichier << "{"
                        << "\"date\":\"" << horodatage << "\", "
                        << "\"temperature\":" << fixed << setprecision(1) << t << ", "
                        << "\"pression\":" << p << ", "
                        << "\"humidite\":" << h
                        << "}" << endl;
                fichier.close();
            }

            this_thread::sleep_for(chrono::seconds(5));
        }
    } catch (const runtime_error &e) {
        cout << "Exception caught: " << e.what() << endl;
    }

    return 0;
}