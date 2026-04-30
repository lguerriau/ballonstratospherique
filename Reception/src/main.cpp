#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>
#include "Modele.h"
#include "Log.h"
#include "Recepteur.h"

// Définition EXPLICITE des broches SPI pour l'ESP32
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23
#define NSS_PIN   5
#define DIO0_PIN  2
#define RST_PIN   14

Modele maBdd;
Log monLog;

// On instancie le module avec ses broches de contrôle
SX1278 radio = new Module(NSS_PIN, DIO0_PIN, RST_PIN);

// On passe la radio au récepteur
Recepteur monRecepteur(&radio, &maBdd, &monLog);

void setup() {
    Serial.begin(115200);
    delay(2000); 
    Serial.println("\n[DEBUG] Demarrage du programme...");

    // === LA CORRECTION EST ICI ===
    // On force l'ESP32 à démarrer son bus SPI sur nos broches exactes
    Serial.println("[DEBUG] Initialisation du bus SPI...");
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, NSS_PIN);
    // =============================

    int state = monRecepteur.initialiser(433.775, 125.0, 12);
    
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("[CRITIQUE] Echec LoRa ! Code d'erreur exact : ");
        Serial.println(state);
        Serial.println(" -> -2  : Puce non trouvee (Verifier NSS, MOSI, MISO, SCK)");
        Serial.println(" -> -16 : Erreur d'ecriture SPI (Bus bloque)");
        while (1);
    }
    
    Serial.println("[OK] Station LoRa prete et en ecoute !");
}

void loop() {
    monRecepteur.ecouterEtTraiter();
}