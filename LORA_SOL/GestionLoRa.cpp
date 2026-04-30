#include "GestionLoRa.h"
#include <SPI.h>

// Constructeur : Initialisation des variables par défaut
GestionLoRa::GestionLoRa() 
    : mes("F4KMN-9", "APLT00", "WIDE1-1", "F4KMN    ", "Hello"), // Utilisation de ta classe Message
      waitForAck(false), 
      lastSentTime(0), 
      lastSentMsgId("") {
}

void GestionLoRa::begin() {
    pinMode(pinLED, OUTPUT);
    digitalWrite(pinLED, LOW);
    Serial.begin(115200); 
    delay(100);
    Serial.println(F("\n--- Gateway LoRa APRS Prête ---"));

    // Initialisation SPI et LoRa
    SPI.begin(5, 19, 27, pinCS); // SCK, MISO, MOSI, CS
    LoRa.setPins(pinCS, pinRST, pinIRQ);

    if (!LoRa.begin(433775000)) {
        Serial.println(F("[ERREUR] LoRa non détecté !"));
        while (true);
    }

    // --- Configuration avancée de la modulation LoRa ---
    LoRa.setSpreadingFactor(12);
    LoRa.setSignalBandwidth(125000);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();
    LoRa.setTxPower(20);
}

void GestionLoRa::process() {
    // 1. Lecture des commandes venant de l'interface Qt (Port Série USB)
    if (Serial.available() > 0) {
        char input = Serial.read();
        while (Serial.available() > 0) Serial.read(); // Nettoyage du buffer

        if (input == 'm') {
            // Envoi de la commande à la nacelle
            mes.setComment(VALID_COMMAND); 
            String pduStr = mes.getPduMes(true); 
            char pduChar[pduStr.length() + 1];
            pduStr.toCharArray(pduChar, sizeof(pduChar));

            sendLoRa(pduChar, strlen(pduChar));

            lastSentMsgId = String(mes.getMessageId());
            waitForAck = true;
            lastSentTime = millis();
            digitalWrite(pinLED, HIGH); 
            Serial.println("LOG:Commande envoyée. Attente ACK...");
        }
        // COMMANDES DE SIMULATION (Pour tester Qt sans faire voler le ballon)
        else if (input == 'e') { Serial.println("ST:en vol"); } 
        else if (input == 'b') { Serial.println("ST:BURST"); } 
        else if (input == 'l') { Serial.println("ST:LANDING"); }
    }

    // 2. Gestion du Timeout d'attente d'ACK
    if (waitForAck && (millis() - lastSentTime > ACK_TIMEOUT)) { 
        Serial.println("LOG:[TIMEOUT] Pas de réponse de la nacelle."); 
        waitForAck = false; 
        digitalWrite(pinLED, LOW); 
    }

    // 3. Appel de la fonction de réception radio LoRa
    receiveLoRa();
}

void GestionLoRa::sendLoRa(char* msg, int length) {
    LoRa.beginPacket();
    LoRa.write('<'); // KISS Header
    LoRa.write(0xFF);
    LoRa.write(0x01);
    LoRa.write((const uint8_t *) msg, length);
    LoRa.endPacket();
}

void GestionLoRa::sendAck(String msgId, String status) {
    // Crée une réponse simple pour la nacelle
    String ackMsg = "F4KMN-9>APLT00,WIDE1-1::NACELLE  :" + status + "{" + msgId;
    char pduAck[ackMsg.length() + 1];
    ackMsg.toCharArray(pduAck, sizeof(pduAck));
    sendLoRa(pduAck, strlen(pduAck));
}

void GestionLoRa::receiveLoRa() {
    int packetSize = LoRa.parsePacket();
    if (packetSize == 0) return; // Rien à lire

    // 1. Lire tout le paquet LoRa
    String incoming = "";
    while (LoRa.available()) {
        incoming += (char)LoRa.read();
    }

    // Retrait de l'en-tête KISS (<0xFF0x01) s'il est présent
    if (incoming.length() >= 3 && incoming.startsWith("<")) {
        incoming = incoming.substring(3);
    }

    // 2. Utilisation de notre classe Message pour le décodage !
    Message messageRecu("", "", "", "", ""); // Objet temporaire pour décoder
    
    if (messageRecu.decode(incoming)) {
        // --- LA TRAME EST VALIDE ET DÉCODÉE ---
        
        // A. Vérification de sécurité : Est-ce bien notre nacelle ?
        if (messageRecu.getCallsign().startsWith(AUTHORIZED_CALLSIGN)) {
            
            String contenu = messageRecu.getComment();

            // B. Est-ce un ACK en réponse à notre commande ?
            if (contenu.startsWith("ACK") && waitForAck) {
                if (String(messageRecu.getMessageId()) == lastSentMsgId) {
                    // C'est le bon ACK ! On envoie le signal à Qt
                    String reponse = "RSSI:" + String(LoRa.packetRssi()) + "|SNR:" + String(LoRa.packetSnr());
                    Serial.println(reponse);
                    waitForAck = false;
                    digitalWrite(pinLED, LOW);
                }
            }
            
            // C. Est-ce un rapport de statut de vol (ST:BURST, etc.) ?
            int stIndex = contenu.indexOf("ST:");
            if (stIndex != -1) {
                Serial.println(contenu.substring(stIndex)); // Envoie "ST:BURST" à Qt
            }

            // D. Est-ce une trame météo BME280 ? (Commence par '_')
            if (contenu.startsWith("_")) {
                int tIndex = contenu.indexOf('t');
                int hIndex = contenu.indexOf('h');
                int bIndex = contenu.indexOf('b');
                
                if (tIndex != -1 && hIndex != -1 && bIndex != -1) {
                    String tempF = contenu.substring(tIndex + 1, tIndex + 4);
                    String hum   = contenu.substring(hIndex + 1, hIndex + 3);
                    String press = contenu.substring(bIndex + 1, bIndex + 6);
                    
                    float tempC = (tempF.toFloat() - 32.0) * 5.0 / 9.0;
                    float pressHpa = press.toFloat() / 10.0;

                    Serial.println("TEMP_EXT:" + String(tempC, 1));
                    Serial.println("HUM:" + hum);
                    Serial.println("PRES:" + String(pressHpa, 1));
                }
            }

            // E. La nacelle a-t-elle demandé un ACK en retour ?
            if (messageRecu.isAckRequested()) {
                sendAck(String(messageRecu.getMessageId()), "ACK");
            }
        }
    } else {
        // La méthode decode() a renvoyé false
        Serial.println("LOG:Trame reçue ignorée (Format APRS non reconnu).");
    }
}