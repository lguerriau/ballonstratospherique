#include "Recepteur.h"

Recepteur::Recepteur(SX1278* r, Modele* bdd, Log* log) 
    : radio(r), laBdd(bdd), erreurLog(log) {}

int Recepteur::initialiser(float freq, float bw, int sf) {
    // Initialisation avec Coding Rate = 5 (4/5) et SyncWord = 0x12
    int state = radio->begin(freq, bw, sf, 5, 0x12); 
    
    if (state == RADIOLIB_ERR_NONE) {
        radio->setCRC(true); // Utile pour filtrer les trames corrompues
    }
    return state; // On retourne l'état réel pour le débogage
}

bool Recepteur::verifierValiditeTrame(const String& trame) {
    return trame.startsWith("F4KMN"); 
}

bool Recepteur::verifierPresenceDonnee(const String& trame) {
    return trame.length() > 10;
}

void Recepteur::ecouterEtTraiter() {
    String str;
    int state = radio->receive(str);

    if (state == RADIOLIB_ERR_NONE) {
        Serial.print("[LORA] Reçu : ");
        Serial.println(str);

        if (verifierValiditeTrame(str)) {            
            if (verifierPresenceDonnee(str)) {       
                laBdd->enregistrerTelemetrie(str);
            } else {                                 
                laBdd->enregistrerTelemetrie("null");
            }
        } else {                                     
            erreurLog->enregistrerErreur("Defaut corrompue");
        }
    }
}