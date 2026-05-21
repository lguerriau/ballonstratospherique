#include "Log.h"
#include <cstring>

bool Log::enregistrerErreur(const char* msg) {

    if (msg == nullptr) return false;
    
    if (strlen(msg) == 0) return false;
    
    Serial.print("[ERREUR LOG] ");
    Serial.println(msg);
    
    return true;
}