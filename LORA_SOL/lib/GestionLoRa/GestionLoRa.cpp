/**
 * @file GestionLoRa.cpp
 * @brief Implémentation de la classe GestionLoRa
 * @version 1.0
 * @author nbrands
 * @date 22/05/2026
 * @details Implémente la logique de pilotage de la puce SX1278 et l'aiguillage 
 * des paquets APRS vers les modules appropriés (Modele, Log).
 */

#include "GestionLoRa.h"

GestionLoRa::GestionLoRa(Modele* bdd, Log* logger) 
    : laBdd(bdd), erreurLog(logger),
      waitForAck(false), lastSentTime(0), lastSentMsgId(0) {
          
    // Règle 6 : Portée lexicale minimale et initialisation sécurisée
    strncpy(authorizedCallsign, "F4KMN", sizeof(authorizedCallsign) - 1);
    authorizedCallsign[sizeof(authorizedCallsign) - 1] = '\0';
    
    strncpy(validCommand, "RSSI/SNR", sizeof(validCommand) - 1);
    validCommand[sizeof(validCommand) - 1] = '\0';

    bool initOk = mes.init("F4KMN-9", "APLT00", "WIDE1-1", "F4KMN    ", "Hello");
    
    if (!initOk && erreurLog != nullptr) {
        erreurLog->enregistrerErreur("ALERTE : Echec de l'initialisation du Message LoRa !");
    }
}

bool GestionLoRa::begin() {
    if (laBdd == nullptr) return false; 
    
    if (erreurLog == nullptr) return false; 

    pinMode(pinLED, OUTPUT);
    digitalWrite(pinLED, LOW);
    
    SPI.begin(5, 19, 27, pinCS); 
    LoRa.setPins(pinCS, pinRST, pinIRQ);

    // Règle 1 & 2 : Pas de "while(1)" infini en cas de panne matérielle
    if (!LoRa.begin(433775000)) {
        erreurLog->enregistrerErreur("Module LoRa introuvable !");
        return false; 
    }
    
    LoRa.setSpreadingFactor(12);
    LoRa.setSignalBandwidth(125000);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();
    LoRa.setTxPower(20);
    
    return true; 
}

void GestionLoRa::process() {
    if (laBdd == nullptr || erreurLog == nullptr) return;

    if (Serial.available() > 0) {
        char input = Serial.read();
        
        for (int i = 0; (i < 64) && (Serial.available() > 0); i++) {
            (void)Serial.read(); 
        }

        if (input == 'm') {
            mes.setComment(validCommand); 
            char pduAEnvoyer[150];
            memset(pduAEnvoyer, 0, sizeof(pduAEnvoyer));
            
            if (mes.getPduMes(true, pduAEnvoyer, sizeof(pduAEnvoyer))) {
                if (sendLoRa(pduAEnvoyer, strlen(pduAEnvoyer))) {
                    lastSentMsgId = mes.getMessageId();
                    waitForAck = true;
                    lastSentTime = millis();
                    digitalWrite(pinLED, HIGH); 
                    Serial.println("LOG: Commande 'm' envoyee. Attente ACK...");
                }
            }
        }
        else if (input == 'e') { Serial.println("ST:en vol"); } 
        else if (input == 'b') { Serial.println("ST:BURST"); } 
        else if (input == 'l') { Serial.println("ST:LANDING"); }
    }

    if (waitForAck && (millis() - lastSentTime > ACK_TIMEOUT)) { 
        erreurLog->enregistrerErreur("[TIMEOUT] Pas de reponse de la nacelle."); 
        waitForAck = false; 
        digitalWrite(pinLED, LOW); 
    }

    bool resRx = receiveLoRa();
    (void)resRx; 
}

bool GestionLoRa::sendLoRa(const char* msg, int length) {
    if (msg == nullptr) return false;
    if (length <= 0 || length >= 150) return false;

    if (LoRa.beginPacket() == 0) return false;

    LoRa.write('<'); 
    LoRa.write(0xFF);
    LoRa.write(0x01);
    
    size_t written = LoRa.write((const uint8_t *) msg, length);
    int endStatus = LoRa.endPacket();

    return (written == (size_t)length && endStatus == 1);
}

bool GestionLoRa::sendAck(int msgId, const char* status) {
    if (msgId < 0 || status == nullptr) return false;

    char pduAck[150];
    memset(pduAck, 0, sizeof(pduAck));
    
    snprintf(pduAck, sizeof(pduAck), "F4KMN-9>APLT00,WIDE1-1::NACELLE  :%s{%d", status, msgId);
    
    return sendLoRa(pduAck, strlen(pduAck));
}

void GestionLoRa::decoderTrameWeather(const char* trame) {
    if (trame == nullptr || strlen(trame) == 0) return;

    const char* tPtr = strchr(trame, 't');
    const char* hPtr = strchr(trame, 'h');
    const char* bPtr = strchr(trame, 'b');

    Serial.println("\n--- DECODAGE WEATHER ---");
    if (tPtr != nullptr) {
        float tempC = (atof(tPtr + 1) - 32.0f) * 5.0f / 9.0f;
        Serial.print("Temperature : "); Serial.print(tempC, 1); Serial.println(" degC");
    }
    if (hPtr != nullptr) {
        int hum = atoi(hPtr + 1);
        if (hum == 0) hum = 100;
        Serial.print("Humidite    : "); Serial.print(hum); Serial.println(" %");
    }
    if (bPtr != nullptr) {
        Serial.print("Pression    : "); Serial.print(atof(bPtr + 1) / 10.0f, 1); Serial.println(" hPa");
    }
    Serial.println("------------------------");
}

bool GestionLoRa::receiveLoRa() {
    int packetSize = LoRa.parsePacket();
    if (packetSize == 0) return true;
    
    char incoming[150];
    memset(incoming, 0, sizeof(incoming));
    int count = 0;

    for (int i = 0; (i < 149) && (LoRa.available() > 0); i++) {
        incoming[i] = (char)LoRa.read();
        count++;
    }
    incoming[count] = '\0';

    const char* tramePropre = incoming;
    if (count >= 3 && incoming[0] == '<') {
        tramePropre = &incoming[3];
    }

    if (strncmp(tramePropre, "ACK_OK", 6) == 0) {
        if (waitForAck) {
            Serial.println("LOG: [SUCCES] ACK recu de la nacelle.");
            Serial.print("RSSI:"); Serial.print(LoRa.packetRssi());
            Serial.print("|SNR:"); Serial.println(LoRa.packetSnr());
            waitForAck = false;
            digitalWrite(pinLED, LOW);
        }
        return true;
    }

    if (strstr(tramePropre, authorizedCallsign) == nullptr) {
        erreurLog->enregistrerErreur("Trame ignoree (Expediteur inconnu).");
        return false;
    }

    laBdd->enregistrerTelemetrie(tramePropre);

    if (strstr(tramePropre, "::") != nullptr) {
        if (mes.decode(tramePropre)) {
            if (mes.isAckRequested()) {
                bool ackSent = sendAck(mes.getMessageId(), "ACK_OK");
                (void)ackSent;
            }
        } else {
            erreurLog->enregistrerErreur("Erreur decodage Message APRS.");
        }
    } else {
        if (strchr(tramePropre, 't') != nullptr && strchr(tramePropre, 'h') != nullptr && strchr(tramePropre, 'b') != nullptr) {
            decoderTrameWeather(tramePropre);
        }
    }
    return true;
}
