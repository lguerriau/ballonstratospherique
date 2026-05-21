#include "Modele.h"
#include <cstring> // Pour strlen

bool Modele::enregistrerTelemetrie(const char* donnee) {
    // Règle 5 (Assertion 1) : Le pointeur est-il valide ?
    if (donnee == nullptr) return false;
    
    // Règle 5 (Assertion 2) : La chaîne est-elle vide ?
    if (strlen(donnee) == 0) return false;
    
    Serial.print("[BDD] Enregistrement de la trame : ");
    Serial.println(donnee);
    
    return true;
}