#include <Arduino.h>
#include "GestionLoRa.h"
#include "Message.h"

// ======================================================
// CONFIGURATION : Commentez la ligne ci-dessous pour repasser en mode normal
#define MODE_TEST_UNITAIRE 
// ======================================================

#ifdef MODE_TEST_UNITAIRE
    // --- CODE POUR LE TEST UNITAIRE DE LA CLASSE MESSAGE ---
    Message testMsg("F4KMN-9", "APLT00", "WIDE1-1", "F4KMN    ", "Hello");

    void setup() {
        Serial.begin(115200);
        delay(8000);
        Serial.println("\n--- [UNIT TEST] CLASSE MESSAGE ACTIVÉ ---");

        // OP 1 : Test format standard
        Serial.print("OP1 - Obtenu : ");
        Serial.println(testMsg.getPduMes(false));

        // OP 2 : Test Troncature et ACK
        testMsg.setRecipient("TROP_LONG_123"); 
        testMsg.setComment("Ceci est un nouveau commentaire");
        Serial.print("OP2 - Obtenu : ");
        Serial.println(testMsg.getPduMes(true)); 
    }

    void loop() {
        // En mode test, on ne fait rien en boucle
    }

#else
    // --- CODE FONCTIONNEL D'ORIGINE ---
    GestionLoRa monLoRa;

    void setup() {
        monLoRa.begin();
    }

    void loop() {
        monLoRa.process();
    }
#endif
