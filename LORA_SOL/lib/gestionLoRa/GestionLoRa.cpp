#include "GestionLoRa.h"

// Constructeur : Initialisation des variables par défaut
//"F4KMN-9" (Callsign) : C'est l'identifiant de ta station (ton indicatif radio). Le -9 est souvent utilisé pour désigner une station mobile (comme ton ballon).
//"APLT00" (Destination) : C'est un code standard APRS. Ici, il indique que les données sont envoyées via un système LoRa.
//"WIDE1-1" (Path) : C'est le "chemin" de répétition. Cela dit aux autres stations relais : "Si vous recevez ce message, répétez-le une fois".
//"F4KMN    " (Recipient) : C'est le destinataire par défaut. Note les espaces : la classe Message attend souvent un nom de 9 caractères. C'est la station au sol (ton PC/Qt) qui va filtrer ces messages.
//"Hello" (Comment) : C'est le texte par défaut qui sera envoyé si tu ne le modifies pas avec setComment().

// Constructeur : Initialisation des variables par défaut
GestionLoRa::GestionLoRa() 
    : mes("F4KMN-9", "APLT00", "WIDE1-1", "F4KMN    ", "Hello"),
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
    receiveLoRa(); // Écoute constante de la radio

    // --- GESTION DES REQUÊTES DEPUIS L'INTERFACE QT ---
    if (Serial.available() > 0) {
        char input = Serial.read();
        
        // On vide le surplus pour éviter les bugs de lecture
        while(Serial.available() > 0) Serial.read(); 

        // A. COMMANDES RÉELLES (Envoi LoRa vers la Nacelle)
        if (input == 'm' || input == 't') {
            digitalWrite(pinLED, HIGH); 
            
            if (input == 'm') mes.setComment(VALID_COMMAND); 
            else mes.setComment("Test message"); 

            char* pdu = mes.getPduMes(true); 
            lastSentMsgId = String(mes.getMessageId()); 

            Serial.println("LOG:Envoi de requête : " + String(pdu)); 
            sendLoRa(pdu, mes.getPduLength()); 

            lastSentTime = millis(); 
            waitForAck = true; 
        }
        
        // B. COMMANDES DE SIMULATION (Pour tester Qt sans faire voler le ballon)
        else if (input == 'e') { Serial.println("ST:en vol"); } 
        else if (input == 'b') { Serial.println("ST:BURST"); } 
        else if (input == 'l') { Serial.println("ST:LANDING"); }
    }

    // Gestion du Timeout
    if (waitForAck && (millis() - lastSentTime > ACK_TIMEOUT)) { 
        Serial.println("LOG:[TIMEOUT] Pas de réponse de la nacelle."); 
        waitForAck = false; 
        digitalWrite(pinLED, LOW); 
    }
}

void GestionLoRa::sendLoRa(char* msg, int length) {
    LoRa.beginPacket();
    LoRa.write('<'); 
    LoRa.write(0xFF);
    LoRa.write(0x01);
    LoRa.write((const uint8_t *) msg, length);
    LoRa.endPacket();
}

void GestionLoRa::sendAck(String msgId, String status) {
    String ackMsg = status + "{" + msgId;
    char pduAck[ackMsg.length() + 1];
    ackMsg.toCharArray(pduAck, sizeof(pduAck));
    sendLoRa(pduAck, strlen(pduAck));
}

void GestionLoRa::receiveLoRa() {
    int packetSize = LoRa.parsePacket();
    if (packetSize == 0) return;

    String incoming = "";
    while (LoRa.available()) {
        incoming += (char)LoRa.read();
    }

    if (incoming.length() >= 3 && incoming.startsWith("<")) {
        incoming = incoming.substring(3);
    }

    // --- CAS 1 : RÉCEPTION D'UN ACK ---
    if (incoming.startsWith("ACK")) {
        int openBrace = incoming.indexOf('{');
        if (openBrace != -1) {
            String ackId = incoming.substring(openBrace + 1);
            if (waitForAck && ackId == lastSentMsgId) {
                // Envoi du RSSI/SNR à Qt
                String reponse = "RSSI:" + String(LoRa.packetRssi()) + "|SNR:" + String(LoRa.packetSnr());
                Serial.println(reponse);
                waitForAck = false;
                digitalWrite(pinLED, LOW);
            }
        }
        return;
    }

    // --- CAS 2 : RÉCEPTION DE TÉLÉMÉTRIE (Nacelle -> Sol) ---
    int arrowIndex = incoming.indexOf('>');
    
    if (arrowIndex != -1) {
        String sender = incoming.substring(0, arrowIndex);
        
        // Sécurité : Est-ce bien notre nacelle ?
        if (sender.startsWith(AUTHORIZED_CALLSIGN)) {
            
            // 1. EXTRACTION DU FLAG DE VOL MPU6050
            // On cherche la balise ":ST:" qu'on a ajoutée dans le code de la nacelle
            int stIndex = incoming.indexOf(":ST:");
            if (stIndex != -1) {
                String statut = incoming.substring(stIndex + 4);
                int braceIndex = statut.indexOf('{'); // Retire l'ID de message s'il y en a un
                if (braceIndex != -1) statut = statut.substring(0, braceIndex);
                statut.trim();
                
                // Envoie à Qt sous le format "ST:BURST"
                Serial.println("ST:" + statut);
            }

            // 2. EXTRACTION DE LA MÉTÉO BME280 (Format APRS)
            // L'APRS météo commence par '_'
            int weatherStart = incoming.indexOf('_');
            if (weatherStart != -1) {
                int tIndex = incoming.indexOf('t', weatherStart);
                int hIndex = incoming.indexOf('h', weatherStart);
                int bIndex = incoming.indexOf('b', weatherStart);
                
                if (tIndex != -1 && hIndex != -1 && bIndex != -1) {
                    String tempF = incoming.substring(tIndex + 1, tIndex + 4);
                    String hum   = incoming.substring(hIndex + 1, hIndex + 3);
                    String press = incoming.substring(bIndex + 1, bIndex + 6);
                    
                    // Conversion Fahrenheit -> Celsius
                    float tempC = (tempF.toFloat() - 32.0) * 5.0 / 9.0;
                    // Conversion de la pression (ex: 10130 -> 1013.0 hPa)
                    float pressHpa = press.toFloat() / 10.0;

                    // Envoi à Qt
                    Serial.println("TEMP_EXT:" + String(tempC, 1));
                    Serial.println("HUM:" + hum);
                    Serial.println("PRES:" + String(pressHpa, 1));
                }
            }

            // 3. RÉPONSE AUTOMATIQUE (ACK) SI REQUÊTE
            int idIndex = incoming.lastIndexOf('{');
            if (incoming.indexOf(VALID_COMMAND) != -1 && idIndex != -1) {
                sendAck(incoming.substring(idIndex + 1), "ACK");
            }
        }
    }
}