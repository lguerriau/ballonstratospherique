/**
 * @file Log.cpp
 * @brief Implémentation de la classe Log
 * @version 1.0
 * @author nbrands
 * @date 22/05/2026
 * @details Implémente la sécurisation des affichages d'erreurs (protection contre les pointeurs nuls).
 */

#include "Log.h"
#include <cstring>

bool Log::enregistrerErreur(const char* msg) {
    if (msg == nullptr) return false;
    
    if (strlen(msg) == 0) return false;
    
    Serial.print("[ERREUR LOG] ");
    Serial.println(msg);
    
    return true;
}