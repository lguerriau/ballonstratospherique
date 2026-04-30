#include "Log.h"

void Log::enregistrerErreur(String message) {
    Serial.print("[ERREUR LOG] ");
    Serial.println(message);
}