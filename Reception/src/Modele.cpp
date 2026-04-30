#include "Modele.h"

void Modele::enregistrerTelemetrie(String donnee) {
    if (donnee == "null") {
        Serial.println("[BDD] Donnée vide reçue, rien à stocker.");
    } else {
        Serial.print("[BDD] Donnée stockée : ");
        Serial.println(donnee);
    }
}