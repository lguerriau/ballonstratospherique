#include <Arduino.h>
#include "GestionLoRa.h"
#include "Message.h"

// ======================================================
// CONFIGURATION : Décommentez la ligne ci-dessous pour activer le mode Sandbox
#define MODE_TEST_UNITAIRE 
// ======================================================

#ifdef MODE_TEST_UNITAIRE
    // Instance de test avec paramètres par défaut
    Message testMsg("F4KMN-9", "APLT00", "WIDE1-1", "F4KMN    ", "Hello");
    bool modeAck = false;

    void afficherEtatEtMenu() {
        Serial.println("\n========= CONFIGURATION DYNAMIQUE DU MESSAGE =========");
        Serial.print("Trame actuelle : "); Serial.println(testMsg.getPduMes(modeAck)); 
        Serial.println("-----------------------------------------------------");
        Serial.print("1. [C]allsign  | 2. [R]ecipient | 3. [M]essage");
        Serial.print("\n4. [A]ck Mode (actuel: "); Serial.print(modeAck ? "ON" : "OFF"); Serial.println(")");
        Serial.println("5. [G]ENERER LA TRAME FINALE");
        Serial.println("=====================================================");
        Serial.println(">>> Tapez une lettre pour configurer :");
    }

    void setup() {
        Serial.begin(115200);
        delay(2000);
        afficherEtatEtMenu();
    }

    void loop() {
        if (Serial.available() > 0) {
            char cmd = Serial.read(); // Lecture de la commande
            while(Serial.available() > 0) Serial.read(); // Nettoyage buffer

            // --- CAS A, B, C : MODIFICATION DES DONNÉES ---
            if (cmd == 'c' || cmd == 'r' || cmd == 'm') {
                Serial.println("\nEntrez la valeur a tester :");
                while (Serial.available() == 0);
                String saisie = Serial.readStringUntil('\n');
                saisie.trim();

                // --- LOGIQUE DE VALIDATION POUR LES PROFS ---
                // Vérification de la limite du destinataire (9 chars)
                if (cmd == 'r' && saisie.length() > 9) {
                    Serial.println("![ALERTE SECURITE] Destinataire trop long (" + String(saisie.length()) + " chars).");
                    Serial.println("! Action : La methode setRecipient() va tronquer a 9 caracteres.");
                }
                
                // Vérification de la limite du commentaire (67 chars)
                if (cmd == 'm' && saisie.length() > 67) {
                    Serial.println("![ALERTE SECURITE] Commentaire trop long (" + String(saisie.length()) + " chars).");
                    Serial.println("! Action : La methode setComment() va tronquer a 67 caracteres.");
                }

                // Exécution des méthodes de la classe Message
                if (cmd == 'c') testMsg.setCallsign(saisie);
                if (cmd == 'r') testMsg.setRecipient(saisie);
                if (cmd == 'm') testMsg.setComment(saisie);
                
                Serial.print(">> Methode executee. Valeur stockee : "); 
                if (cmd == 'r') Serial.println(saisie.substring(0,9));
                else if (cmd == 'm') Serial.println(saisie.substring(0,67));
                else Serial.println(saisie);
            } 
            // --- CAS D : BASCULEMENT DU MODE ACK ---
            else if (cmd == 'a') {
                modeAck = !modeAck;
                Serial.print(">> Mode ACK change en : "); Serial.println(modeAck ? "ON" : "OFF");
            } 
            // --- CAS E : GÉNÉRATION FINALE DU PDU ---
            else if (cmd == 'g') {
                Serial.println("\n--- RESULTAT DE LA METHODE getPduMes() ---");
                Serial.println(testMsg.getPduMes(modeAck));
                Serial.println("------------------------------------------");
            }

            delay(800);
            afficherEtatEtMenu();
        }
    }

#else
    // --- CODE FONCTIONNEL (MODE PASSERELLE) ---
    GestionLoRa monLoRa;
    void setup() { monLoRa.begin(); }
    void loop() { monLoRa.process(); }       
#endif