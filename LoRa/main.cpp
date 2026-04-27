#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>   
#include <cmath>

#include "../BME280/bme280.h"
#include "../LM75/LM75.h"
#include "RadioLib.h"
#include "RadioLib/src/hal/RPi/PiHal.h"

using namespace std;

const string INDICATIF = "F4KMN-9"; 
const float FREQUENCE  = 433.775; 
const int SF = 12;                
const float BW = 125.0;           

PiHal* hal = new PiHal(0); 
SX1278 lora = new Module(hal, 8, 4, 17, 18);

// =========================================================
// Fonction 1 : Horodatage
// =========================================================
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

// =========================================================
// Fonction 2 : Validation des limites (Sécurité capteurs)
// =========================================================
bool validerDonneesCapteurs(float tempF, float hum, float press) {
    // Limites arbitraires à ajuster selon ton besoin (Ex: Température entre -50F et 150F)
    if (tempF < -50.0 || tempF > 150.0) return false;
    if (hum < 0.0 || hum > 100.0) return false;
    if (press < 800.0 || press > 1100.0) return false; // Pression terrestre normale
    
    return true;
}

// =========================================================
// Fonction 3 : Construction de la trame
// =========================================================
string buildAPRSTrame(float tempF, float hum, float press) {
    // Vérification des limites avant construction
    if (!validerDonneesCapteurs(tempF, hum, press)) {
        return "ERREUR : Valeur(s) capteur(s) hors limites !";
    }

    stringstream aprs;
    aprs << INDICATIF << ">APRS,WIDE1-1:_" << getAPRSTimestamp();
    aprs << "c...s...g..."; 
    aprs << "t" << setfill('0') << setw(3) << (int)round(tempF);
    
    int h_int = (int)round(hum);
    aprs << "h" << setfill('0') << setw(2) << (h_int >= 100 ? 0 : h_int);
    aprs << "b" << setfill('0') << setw(5) << (int)round(press * 10.0);
    
    return aprs.str();
}

// =========================================================
// PROGRAMME PRINCIPAL
// =========================================================
int main() {
    // Initialisation Radio
    int state = lora.begin(FREQUENCE, BW, SF, 5, 0x12);
    if (state != RADIOLIB_ERR_NONE) {
        cout << "Erreur init LoRa : " << state << endl;
        // return -1; // Commenté temporairement si tu testes sur un PC sans le module branché
    } else {
        lora.setSyncWord(0x12);
        lora.setCRC(true);
    }

    int choix = 0;

    while (true) {
        cout << "\n=========================================" << endl;
        cout << "         MENU STATION APRS" << endl;
        cout << "=========================================" << endl;
        cout << "1. Mode Production (Capteurs et Envoi continus)" << endl;
        cout << "2. Mode Test Unitaire (Génération de Trame)" << endl;
        cout << "3. Mode Test Unitaire (Envoi d'une trame de test via LoRa)" << endl;
        cout << "4. Quitter" << endl;
        cout << "Votre choix : ";
        
        if (!(cin >> choix)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (choix == 1) {
            // ---------------------------------------------------------
            // MODE PRODUCTION (CAPTEURS RÉELS)
            // ---------------------------------------------------------
            cout << "\n[SYSTEME] Mode Production activé. (CTRL+C pour arrêter)" << endl;
            try {
                BME280 capteurBME(0x77); 
                LM75   capteurLM(0x48);   

                while (true) {
                    float hum   = capteurBME.obtenirHumidite();
                    float press = capteurBME.obtenirPression();
                    float tempC_LM = capteurLM.getTemperature();
                    float tempF_LM = (tempC_LM * 1.8) + 32; 

                    string trame = buildAPRSTrame(tempF_LM, hum, press);
                    
                    if (trame.find("ERREUR") != string::npos) {
                        cout << "Anomalie Capteur ignorée : " << trame << endl;
                    } else {
                        cout << "Transmission : " << trame << " ... ";
                        state = lora.transmit(trame.c_str());
                        if (state == RADIOLIB_ERR_NONE) cout << "OK" << endl;
                        else cout << "Erreur " << state << endl;
                    }

                    this_thread::sleep_for(chrono::seconds(30));
                }
            } catch (const exception &e) {
                cerr << "Erreur Matérielle : " << e.what() << endl;
            }

        } else if (choix == 2) {
            // ---------------------------------------------------------
            // MODE TEST UNITAIRE (INTERACTIF)
            // ---------------------------------------------------------
            cout << "\n--- TEST FORMATAGE APRS ---" << endl;
            float testTempF, testHum, testPress;

            cout << "Entrez une temperature (en F, ex: 72.0) : ";
            cin >> testTempF;
            cout << "Entrez une humidite (en %, ex: 45.0) : ";
            cin >> testHum;
            cout << "Entrez une pression (en hPa, ex: 1013.2) : ";
            cin >> testPress;

            string trameTest = buildAPRSTrame(testTempF, testHum, testPress);
            
            cout << "\n>> Trame générée : " << trameTest << endl;

            if (trameTest.find("ERREUR") != string::npos) {
                cout << "-> RESULTAT : ÉCHEC (Les valeurs sont hors limites, la sécurité a fonctionné !)" << endl;
            } else if (trameTest.find(INDICATIF) != string::npos) {
                cout << "-> RESULTAT : SUCCÈS (La trame est formatée)" << endl;
            } else {
                cout << "-> RESULTAT : ÉCHEC (Problème de formatage inconnu)" << endl;
            }

        } else if (choix == 3) {
            // ---------------------------------------------------------
            // MODE TEST UNITAIRE (ENVOI RADIO PHYSIQUE)
            // ---------------------------------------------------------
            cout << "\n--- SOUS-MENU : TEST TRANSMISSION LORA ---" << endl;
            cout << "1. Envoyer une trame valide (Generee par le programme)" << endl;
            cout << "2. Envoyer une trame incomplete (Champs manquants)" << endl;
            cout << "3. Envoyer une trame invalide (Caracteres speciaux hors protocole)" << endl;
            cout << "4. Envoyer une trame vide" << endl;
            cout << "Votre choix : ";
            
            int sousChoix = 0;
            if (!(cin >> sousChoix)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Saisie incorrecte, retour au menu principal." << endl;
                continue;
            }

            string messageTest = "";

            if (sousChoix == 1) {
                // On utilise ta fonction buildAPRSTrame pour avoir une vraie trame valide
                messageTest = buildAPRSTrame(72.0, 45.0, 1013.2); 
            } else if (sousChoix == 2) {
                messageTest = "F4KMN-9>APRS,WIDE1-1:_t072"; // Manque l'humidité et la pression
            } else if (sousChoix == 3) {
                messageTest = "@@@TRAME_#INVALIDE_!!!***"; // Format qui fera planter la réception
            } else if (sousChoix == 4) {
                messageTest = ""; // Rien du tout
            } else {
                cout << "Choix inexistant, annulation." << endl;
                continue;
            }
            
            cout << "\nTentative d'envoi de la trame : [" << messageTest << "] (" << messageTest.length() << " octets) ... ";
            
            // Envoi matériel via la classe LoRa
            int state = lora.transmit(messageTest.c_str());

            if (state == RADIOLIB_ERR_NONE) {
                cout << "-> RESULTAT : SUCCÈS (Le module a bien émis sur l'antenne)" << endl;
            } else {
                cout << "-> RESULTAT : ÉCHEC (Code d'erreur matériel LoRa : " << state << ")" << endl;
            }

        } else if (choix == 4) {
            cout << "Arrêt du système." << endl;
            break;
        }
    }

    return 0;
}