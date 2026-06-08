// --- src/Modele.cpp ---
#include "Modele.h"

void Modele::enregistrerTelemetrie(String donnee) {
    if (donnee.length() == 0) return;
    
    Serial.print("[BDD] Enregistrement de la trame : ");
    Serial.println(donnee);
}