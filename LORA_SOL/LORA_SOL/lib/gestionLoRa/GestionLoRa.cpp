#include "GestionLoRa.h"

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

    Serial.println(F("\n--- Gateway LoRa APRS Prête ---"));

    // Initialisation SPI et LoRa
    SPI.begin(5, 19, 27, pinCS); // SCK, MISO, MOSI, CS
    LoRa.setPins(pinCS, pinRST, pinIRQ);

    if (!LoRa.begin(433775000)) {
        Serial.println(F("[ERREUR] LoRa non détecté !"));
        while (true);
    }

    LoRa.setSpreadingFactor(12);
    LoRa.setSignalBandwidth(125000);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();
    LoRa.setTxPower(20);
}

void GestionLoRa::process() {
    receiveLoRa(); // Écoute constante

    // Gestion du Timeout d'ACK
    if (waitForAck && (millis() - lastSentTime > ACK_TIMEOUT)) {
        Serial.println(">>> [TIMEOUT] Pas d'ACK reçu pour l'ID " + lastSentMsgId);
        waitForAck = false;
        digitalWrite(pinLED, LOW);
    }

    // Gestion des commandes série (Qt)
    if (Serial.available() > 0) {
        char input = Serial.read();
        while(Serial.available() > 0) Serial.read(); 

        if (input == 'm' || input == 't') {
            digitalWrite(pinLED, HIGH);
            if (input == 'm') mes.setComment(VALID_COMMAND);
            else mes.setComment("Test message");

            char* pdu = mes.getPduMes(true); 
            lastSentMsgId = String(mes.getMessageId());

            Serial.println("Envoi LoRa : " + String(pdu));
            sendLoRa(pdu, mes.getPduLength());

            lastSentTime = millis();
            waitForAck = true;
        }
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
    
    Serial.print(F(">>> Réponse envoyée : "));
    Serial.println(ackMsg);
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

    // CAS 1 : ACK reçu
    if (incoming.startsWith("ACK")) {
        int openBrace = incoming.indexOf('{');
        if (openBrace != -1) {
            String ackId = incoming.substring(openBrace + 1);
            if (waitForAck && ackId == lastSentMsgId) {
                if (incoming.startsWith("ACK{")) {
                    Serial.print("RSSI:");
                    Serial.print(LoRa.packetRssi());
                    Serial.print("|SNR:");
                    Serial.println(LoRa.packetSnr());
                } else {
                    Serial.print(F(">>> [ERREUR DISTANTE] : "));
                    Serial.println(incoming.substring(0, openBrace));
                }
                waitForAck = false;
                digitalWrite(pinLED, LOW);
            }
        }
        return;
    }

    // CAS 2 : Requête entrante
    int arrowIndex = incoming.indexOf('>');
    int idIndex = incoming.lastIndexOf('{');
    int colonIndex = incoming.lastIndexOf(':');

    if (arrowIndex != -1 && idIndex != -1) {
        String sender = incoming.substring(0, arrowIndex);
        String command = incoming.substring(colonIndex + 1, idIndex);
        command.trim();
        String msgId = incoming.substring(idIndex + 1);

        if (!sender.startsWith(AUTHORIZED_CALLSIGN)) {
            sendAck(msgId, "ACK_CALL");
            return;
        }

        if (command == VALID_COMMAND) {
            sendAck(msgId, "ACK");
        } else {
            sendAck(msgId, "ACK_CMD");
        }
    }
}