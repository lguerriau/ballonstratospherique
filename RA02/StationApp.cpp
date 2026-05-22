#include "StationApp.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <cassert>

// Inclusion des classes capteurs
#include "../BME280/bme280.h"
#include "../LM75/LM75.h"

using namespace std;

StationApp::StationApp() : builder("F4KMN-9") {
    assert(cin.good());
    assert(cout.good());
}

void StationApp::demarrer() {
    assert(cin.good());
    assert(cout.good());

    cout << "Initialisation du module LoRa (SPI)..." << endl;
    if (!radio.initialiser()) {
        cout << "[ALERTE] Probleme d'initialisation LoRa. Verifie le cablage et le mode sudo." << endl;
    } else {
        cout << "[OK] LoRa initialise sur le port SPI CE0." << endl;
    }

    int choix = 0;
    while (true) {
        cout << "\n=========================================" << endl;
        cout << "         MENU STATION APRS (RPI)" << endl;
        cout << "=========================================" << endl;
        cout << "1. Mode Production (Capteurs continus)" << endl;
        cout << "2. Lecture directe du capteur LM75" << endl;
        cout << "3. Mode Test Formatage (Weather)" << endl;
        cout << "4. Mode Test Transmission (Weather)" << endl;
        cout << "5. Quitter" << endl;
        cout << "Votre choix : ";
        
        if (!(cin >> choix)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch(choix) {
            case 1: modeProduction(); break;
            case 2: lireCapteurLM75(); break;
            case 3: testFormatage(); break;
            case 4: testTransmission(); break;
            case 5: cout << "Arret." << endl; return;
            default: cout << "Choix invalide." << endl;
        }
    }
}

void StationApp::modeProduction() {
    assert(cin.good());
    assert(cout.good());

    cout << "\n[SYSTEME] Mode Production active. (CTRL+C pour arreter)" << endl;
    
    // Initialisation des capteurs (une seule fois au debut du mode)
    BME280 capteurBME(0x77); 
    LM75   capteurLM(0x48);   

    while (true) {
        // Recuperation des donnees brutes
        float hum = capteurBME.obtenirHumidite();
        float press = capteurBME.obtenirPression();

        float tempC_LM = capteurLM.getTemperature();
        float tempF_LM = (tempC_LM * 1.8) + 32; 

        // Construction de la trame Weather
        string trame = builder.buildTrame(tempF_LM, hum, press);
        
        if (trame.find("ERREUR") != string::npos) {
            cout << "[ALERTE] " << trame << " (T:" << tempF_LM << "F, H:" << hum << "%, P:" << press << "hPa)" << endl;
        } else {
            cout << "Envoi LoRa : " << trame << endl;
            radio.envoyer(trame);
        }

        this_thread::sleep_for(chrono::seconds(30));
    }
}

/**
 * @brief Réalise une lecture répétée du capteur de température LM75 seul.
 */
void StationApp::lireCapteurLM75() {
    assert(cout.good());
    cout << "\n[SYSTEME] Lecture du capteur LM75 seul. (CTRL+C pour arreter)" << endl;
    
    try {
        LM75 capteurLM(0x48);
        while (true) {
            float tempC = capteurLM.getTemperature();
            cout << "Temperature LM75 : " << fixed << setprecision(3) << tempC << " °C" << endl;
            this_thread::sleep_for(chrono::seconds(1));
        }
    } catch (const runtime_error &e) {
        cout << "Exception caught : " << e.what() << endl;
    }
}

void StationApp::testFormatage() {
    assert(cin.good());
    assert(cout.good());

    cout << "\n--- TEST FORMATAGE APRS WEATHER ---" << endl;
    
    float temp = 0.0f;
    int essaisTemp = 0;
    cout << "Entrez la temperature (-45 a 140 °F) : ";
    while (!(cin >> temp) || temp < -45.0f || temp > 140.0f) {
        essaisTemp++;
        if (essaisTemp >= 5) {
            cout << "[ERREUR] Trop d'echecs de saisie pour la temperature." << endl;
            return;
        }
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Temperature invalide. \nReessayez : ";
    }
    
    float hum = 0.0f;
    int essaisHum = 0;
    cout << "Entrez l'humidité (0 a 100%) : ";
    while (!(cin >> hum) || hum < 0.0f || hum > 100.0f) {
        essaisHum++;
        if (essaisHum >= 5) {
            cout << "[ERREUR] Trop d'echecs de saisie pour l'humidite." << endl;
            return;
        }
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Humidite invalide. \nReessayez : ";
    }
    
    float press = 0.0f;
    int essaisPress = 0;
    cout << "Entrez la pression (800 a 1100 hPa) : ";
    while (!(cin >> press) || press < 800.0f || press > 1100.0f) {
        essaisPress++;
        if (essaisPress >= 5) {
            cout << "[ERREUR] Trop d'echecs de saisie pour la pression." << endl;
            return;
        }
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Pression atmospherique invalide.\nReessayez : ";
    }

    string trameTest = builder.buildTrame(temp, hum, press); 
    cout << ">> Trame generee : " << trameTest << endl;
}

void StationApp::testTransmission() {
    assert(cin.good());
    assert(cout.good());

    cout << "\n--- TEST TRANSMISSION LORA (WEATHER) ---" << endl;
    
    float temp = 0.0f;
    int essaisTemp = 0;
    cout << "Entrez la temperature (-45 a 140 °F) : ";
    while (!(cin >> temp) || temp < -45.0f || temp > 140.0f) {
        essaisTemp++;
        if (essaisTemp >= 5) {
            cout << "[ERREUR] Trop d'echecs de saisie pour la temperature." << endl;
            return;
        }
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Temperature invalide. \nReessayez : ";
    }
    
    float hum = 0.0f;
    int essaisHum = 0;
    cout << "Entrez l'humidite (0 a 100 %) : ";
    while (!(cin >> hum) || hum < 0.0f || hum > 100.0f) {
        essaisHum++;
        if (essaisHum >= 5) {
            cout << "[ERREUR] Trop d'echecs de saisie pour l'humidite." << endl;
            return;
        }
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Humidite invalide. \nReessayez : ";
    }
    
    float press = 0.0f;
    int essaisPress = 0;
    cout << "Entrez la pression (800 a 1100 hPa) : ";
    while (!(cin >> press) || press < 800.0f || press > 1100.0f) {
        essaisPress++;
        if (essaisPress >= 5) {
            cout << "[ERREUR] Trop d'echecs de saisie pour la pression." << endl;
            return;
        }
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Pression atmospherique invalide. \nReessayez : ";
    }

    string messageTest = builder.buildTrame(temp, hum, press);
    
    cout << "Envoi de : " << messageTest << endl;
    radio.envoyer(messageTest);
    
    this_thread::sleep_for(chrono::seconds(2));
}