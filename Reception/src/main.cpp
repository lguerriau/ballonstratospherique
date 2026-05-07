#include <Arduino.h>
#include "Modele.h"
#include "Log.h"
#include "Recepteur.h"

Modele maBdd;
Log monLog;
Recepteur monRecepteur(&maBdd, &monLog);

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=========================================");
    Serial.println("   STATION SOL LORA       ");
    Serial.println("=========================================");

    if (!monRecepteur.initialiser()) {
        Serial.println("[CRITIQUE] LoRa non détecté !");
        Serial.println("-> Vérifiez les branchements (CS, RST, DIO0/IRQ).");
        while (true); // On bloque ici si panne matérielle
    }

    Serial.println("[OK] Puce LoRa initialisée.");
    Serial.println("[OK] Écoute silencieuse en cours...\n");
}

void loop() {
    // Cette fonction ne bloque pas le programme. 
    // Elle vérifie le ciel en une fraction de seconde et passe à la suite.
    monRecepteur.ecouterEtRepondre();
    
    // Petit délai pour laisser souffler l'ESP32
    delay(10);
}