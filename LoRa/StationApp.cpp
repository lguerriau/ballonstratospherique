#include "StationApp.h"
#include <iostream>
#include <thread>
#include <chrono>

// Inclusions des capteurs (Attention à bien garder tes chemins relatifs)
#include "../BME280/bme280.h"
#include "../LM75/LM75.h"

using namespace std;

StationApp::StationApp() : radio(433.775, 125.0, 12), builder("F4KMN-9") {}

void StationApp::demarrer() {
    radio.initialiser();

    int choix = 0;
    while (true) {
        cout << "\n=========================================" << endl;
        cout << "         MENU STATION APRS" << endl;
        cout << "=========================================" << endl;
        cout << "1. Mode Production (Capteurs et Envoi continus)" << endl;
        cout << "2. Mode Test Unitaire (Génération de Trame)" << endl;
        cout << "3. Mode Test Unitaire (Envoi LoRa)" << endl;
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
            case 4: 
                cout << "Arrêt du système." << endl; 
                return;
            default: 
                cout << "Choix invalide." << endl;
        }
    }
}

void StationApp::modeProduction() {
    cout << "\n[SYSTEME] Mode Production activé. (CTRL+C pour arrêter)" << endl;
    
    try {
        // 1. Initialisation des capteurs une seule fois HORS de la boucle
        BME280 capteurBME(0x77); 
        LM75   capteurLM(0x48);   

        // Petit délai pour laisser le temps au BME280 de se stabiliser
        this_thread::sleep_for(chrono::milliseconds(500));

        while (true) {
            // 2. Récupération des données
            float hum   = capteurBME.obtenirHumidite();
            float pressRaw = capteurBME.obtenirPression();
            
            // 3. CONVERSION cmHg -> hPa (multiplier par 13.3322)
            float press = pressRaw * 13.3322;

            float tempC_LM = capteurLM.getTemperature();
            float tempF_LM = (tempC_LM * 1.8) + 32; 

            // 4. Construction de la trame avec la pression corrigée
            string trame = builder.buildTrame(tempF_LM, hum, press);
            
            if (trame.find("ERREUR") != string::npos) {
                // Le debug affichera maintenant les bonnes unités
                cout << "[ALERTE] " << trame << endl;
            } else {
                cout << "Transmission : " << trame << " ... ";
                int state = radio.envoyer(trame);
                if (state == RADIOLIB_ERR_NONE) cout << "OK" << endl;
                else cout << "Erreur Radio " << state << endl;
            }

            // Pause de 30 secondes entre chaque envoi
            this_thread::sleep_for(chrono::seconds(30));
        }
    } catch (const exception &e) {
        cerr << "Erreur Matérielle Critique : " << e.what() << endl;
    }
}

void StationApp::testFormatage() {
    cout << "\n--- TEST FORMATAGE APRS ---" << endl;
    float testTempF, testHum, testPress;

    cout << "Entrez une temperature (en °F, ex: 72.0) : ";
    cin >> testTempF;
    cout << "Entrez une humidite (en %, ex: 45.0) : ";
    cin >> testHum;
    cout << "Entrez une pression (en hPa, ex: 1013.2) : ";
    cin >> testPress;

    string trameTest = builder.buildTrame(testTempF, testHum, testPress);
    cout << "\n>> Trame générée : " << trameTest << endl;

    if (trameTest.find("ERREUR") != string::npos) {
        cout << "-> RESULTAT : ÉCHEC (Les valeurs sont hors limites)" << endl;
    } else if (trameTest.find("F4KMN-9") != string::npos) {
        cout << "-> RESULTAT : SUCCÈS (La trame est formatée)" << endl;
    } else {
        cout << "-> RESULTAT : ÉCHEC (Problème inconnu)" << endl;
    }
}

void StationApp::testTransmission() {
    cout << "\n--- SOUS-MENU : TEST TRANSMISSION LORA ---" << endl;
    cout << "1. Envoyer une trame valide" << endl;
    cout << "2. Envoyer une trame incomplete" << endl;
    cout << "3. Envoyer une trame invalide" << endl;
    cout << "4. Envoyer une trame vide" << endl;
    cout << "Votre choix : ";
    
    int sousChoix = 0;
    if (!(cin >> sousChoix)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Saisie incorrecte." << endl;
        return;
    }

    string messageTest = "";
    if (sousChoix == 1) messageTest = builder.buildTrame(72.0, 45.0, 1013.2); 
    else if (sousChoix == 2) messageTest = "F4KMN-9>APRS,WIDE1-1:_t072"; 
    else if (sousChoix == 3) messageTest = "@@@TRAME_#INVALIDE_!!!***"; 
    else if (sousChoix == 4) messageTest = ""; 
    else {
        cout << "Choix inexistant, annulation." << endl;
        return;
    }
    
    cout << "\nTentative d'envoi de la trame : [" << messageTest << "] (" << messageTest.length() << " octets) ... ";
    int state = radio.envoyer(messageTest);

    if (state == RADIOLIB_ERR_NONE) {
        cout << "-> RESULTAT : SUCCÈS (Le module a bien émis sur l'antenne)" << endl;
    } else {
        cout << "-> RESULTAT : ÉCHEC (Code d'erreur LoRa : " << state << ")" << endl;
    }
}