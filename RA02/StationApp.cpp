#include "StationApp.h"
#include <iostream>
#include <thread>
#include <chrono>

// Inclusion des classes capteurs
#include "../BME280/bme280.h"
#include "../LM75/LM75.h"

using namespace std;

StationApp::StationApp() : builder("F4KMN-9") {}

void StationApp::demarrer() {
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
        cout << "2. Mode Test Formatage (Weather)" << endl;
        cout << "3. Mode Test Transmission (Weather)" << endl;
        cout << "4. Quitter" << endl;
        cout << "Votre choix : ";
        
        if (!(cin >> choix)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch(choix) {
            case 1: modeProduction(); break;
            case 2: testFormatage(); break;
            case 3: testTransmission(); break;
            case 4: cout << "Arret." << endl; return;
            default: cout << "Choix invalide." << endl;
        }
    }
}

void StationApp::modeProduction() {
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

void StationApp::testFormatage() {
    cout << "\n--- TEST FORMATAGE APRS WEATHER ---" << endl;
    
    float temp, hum, press;
    
    // Validation Température (-60.0 à 140.0 °F)
    cout << "Entrez la temperature (-60 a 140 °F) : ";
    while (!(cin >> temp) || temp < -60.0f || temp > 140.0f) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Temperature invalide. " << endl;
        cout << "Reessayez : ";
    }
    
    // Validation Humidité (0.0 à 100.0 %)
    cout << "Entrez l'humidité (0 a 100%) : ";
    while (!(cin >> hum) || hum < 0.0f || hum > 100.0f) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Humidite invalide. " << endl;
        cout << "Reessayez : ";
    }
    
    // Validation Pression (800.0 à 1100.0 hPa)
    cout << "Entrez la pression (800 a 1100 hPa) : ";
    while (!(cin >> press) || press < 800.0f || press > 1100.0f) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Pression atmospherique invalide." << endl;
        cout << "Reessayez : ";
    }

    string trameTest = builder.buildTrame(temp, hum, press); 
    cout << ">> Trame generee : " << trameTest << endl;
}

void StationApp::testTransmission() {
    cout << "\n--- TEST TRANSMISSION LORA (WEATHER) ---" << endl;
    
    float temp, hum, press;
    
    // Validation Température (-60.0 à 140.0 °F)
    cout << "Entrez la temperature (-60 a 140 °F) : ";
    while (!(cin >> temp) || temp < -60.0f || temp > 140.0f) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Temperature invalide. " << endl;
        cout << "Reessayez : ";
    }
    
    // Validation Humidité (0.0 à 100.0 %)
    cout << "Entrez l'humidite (0 a 100 %) : ";
    while (!(cin >> hum) || hum < 0.0f || hum > 100.0f) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Humidite invalide. " << endl;
        cout << "Reessayez : ";
    }
    
    // Validation Pression (800.0 à 1100.0 hPa)
    cout << "Entrez la pression (800 a 1100 hPa) : ";
    while (!(cin >> press) || press < 800.0f || press > 1100.0f) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "[ALERTE] Pression atmospherique invalide. " << endl;
        cout << "Reessayez : ";
    }

    string messageTest = builder.buildTrame(temp, hum, press);
    
    cout << "Envoi de : " << messageTest << endl;
    radio.envoyer(messageTest);
    
    this_thread::sleep_for(chrono::seconds(2));
}