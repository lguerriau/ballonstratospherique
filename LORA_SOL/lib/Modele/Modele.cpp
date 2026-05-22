/**
 * @file Modele.cpp
 * @brief Implémentation de la classe Modele
 * @version 1.0
 * @author nbrands
 * @date 22/05/2026
 * @details Contient la logique d'enregistrement en base de données.
 * Intègre des assertions de sécurité pour éviter les plantages mémoire.
 */

#include "Modele.h"
#include <cstring> 

bool Modele::enregistrerTelemetrie(const char* donnee) {
    // Règle 5 (Assertion 1) : Le pointeur est-il valide ?
    if (donnee == nullptr) return false;
    
    // Règle 5 (Assertion 2) : La chaîne est-elle vide ?
    if (strlen(donnee) == 0) return false;
    
    Serial.print("[BDD] Enregistrement de la trame : ");
    Serial.println(donnee);
    
    return true;
}