#include "GestionLoRa.h"

// Le constructeur initialise tes outils ET ceux du collègue
GestionLoRa::GestionLoRa(Modele* bdd, Log* logger) 
    : laBdd(bdd), erreurLog(logger),
      mes("F4KMN-9", "APLT00", "WIDE1-1", "F4KMN    ", "Hello"), 
      waitForAck(false), 
      lastSentTime(0), 
      lastSentMsgId("") {
}

void GestionLoRa::begin() {
    pinMode(pinLED, OUTPUT);
    digitalWrite(pinLED, LOW);
    
    SPI.begin(5, 19, 27, pinCS); 
    LoRa.setPins(pinCS, pinRST, pinIRQ);

    if (!LoRa.begin(433775000)) {
        erreurLog->enregistrerErreur("Module LoRa introuvable !");
        while (1);
    }
    
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
            // Envoi de la requête manuelle (ex: RSSI/SNR) à la nacelle
            mes.setComment(VALID_COMMAND); 
            
            // On génère la trame avec demande d'ACK (true)
            char* pduAEnvoyer = mes.getPduMes(true); 

            // On envoie physiquement la trame via LoRa
            sendLoRa(pduAEnvoyer, strlen(pduAEnvoyer));

            // On mémorise l'état pour le Timeout
            lastSentMsgId = String(mes.getMessageId());
            waitForAck = true;
            lastSentTime = millis();
            digitalWrite(pinLED, HIGH); 
            
            Serial.println("LOG: Commande 'm' envoyée. Attente ACK...");
        }
        // COMMANDES DE SIMULATION (Pour tester Qt sans faire voler le ballon)
        else if (input == 'e') { Serial.println("ST:en vol"); } 
        else if (input == 'b') { Serial.println("ST:BURST"); } 
        else if (input == 'l') { Serial.println("ST:LANDING"); }
    }

    // 2. Gestion du Timeout (si on n'a pas reçu d'ACK à temps)
    if (waitForAck && (millis() - lastSentTime > ACK_TIMEOUT)) { 
        // On utilise le Log du collègue pour signaler l'erreur !
        erreurLog->enregistrerErreur("[TIMEOUT] Pas de réponse de la nacelle."); 
        waitForAck = false; 
        digitalWrite(pinLED, LOW); 
    }

    // 3. Écoute permanente des ondes LoRa
    receiveLoRa();
}

void GestionLoRa::sendLoRa(char* msg, int length) {
    LoRa.beginPacket();
    LoRa.write('<'); // En-tête KISS
    LoRa.write(0xFF);
    LoRa.write(0x01);
    LoRa.write((const uint8_t *) msg, length);
    LoRa.endPacket();
}

void GestionLoRa::sendAck(String msgId, String status) {
    String ackMsg = "F4KMN-9>APLT00,WIDE1-1::NACELLE  :" + status + "{" + msgId;
    char pduAck[ackMsg.length() + 1];
    ackMsg.toCharArray(pduAck, sizeof(pduAck));
    sendLoRa(pduAck, strlen(pduAck));
}

// Fonction du collègue adaptée
void GestionLoRa::decoderTrameWeather(const String& trame) {
    int tPos = trame.indexOf('t');
    int hPos = trame.indexOf('h');
    int bPos = trame.indexOf('b');

    Serial.println("\n--- DÉCODAGE WEATHER ---");
    if (tPos != -1) {
        float tempF = trame.substring(tPos + 1, tPos + 4).toFloat();
        float tempC = (tempF - 32.0) * 5.0 / 9.0;
        Serial.print("Température : "); Serial.print(tempC, 1); Serial.println(" °C");
    }
    if (hPos != -1) {
        int hum = trame.substring(hPos + 1, hPos + 3).toInt();
        if (hum == 0) hum = 100;
        Serial.print("Humidité    : "); Serial.print(hum); Serial.println(" %");
    }
    if (bPos != -1) {
        float press = trame.substring(bPos + 1, bPos + 6).toFloat() / 10.0;
        Serial.print("Pression    : "); Serial.print(press, 1); Serial.println(" hPa");
    }
    Serial.println("------------------------");
}

void GestionLoRa::receiveLoRa() {
    int packetSize = LoRa.parsePacket();
    if (packetSize == 0) return;

    String incoming = "";
    while (LoRa.available()) {
        incoming += (char)LoRa.read();
    }

    if (incoming.length() >= 3 && incoming.startsWith("<")) {
        incoming = incoming.substring(3); // Retrait KISS
    }

    // --- 1. GESTION DES ACKS SIMPLES (Pour éviter les erreurs dans le Log) ---
    if (incoming.startsWith("ACK_OK")) {
        if (waitForAck) {
            Serial.println("LOG: [SUCCES] ACK recu de la nacelle.");
            String reponse = "RSSI:" + String(LoRa.packetRssi()) + "|SNR:" + String(LoRa.packetSnr());
            Serial.println(reponse);
            waitForAck = false;
            digitalWrite(pinLED, LOW);
        }
        return; // On arrête le traitement ici pour tous les ACK_OK
    }

    // --- 2. VÉRIFICATION DE SÉCURITÉ ---
    // Si la trame ne vient pas de F4KMN, on l'ignore
    if (incoming.indexOf(AUTHORIZED_CALLSIGN) == -1) {
        erreurLog->enregistrerErreur("Trame ignoree (Expediteur inconnu). Trame: " + incoming);
        return;
    }

    // --- 3. ACTION COLLÈGUE : Enregistrement BDD ---
    // À ce stade, la trame est sûre, on la sauvegarde.
    laBdd->enregistrerTelemetrie(incoming);

    // --- 4. AIGUILLAGE INTELLIGENT ---
    if (incoming.indexOf("::") != -1) {
        // CAS A : C'est un "Message APRS" ciblé (Tes commandes)
        Message messageRecu("", "", "", "", "");
        
        if (messageRecu.decode(incoming)) {
            // Envoi de l'ACK si demandé
            if (messageRecu.isAckRequested()) {
                sendAck(String(messageRecu.getMessageId()), "ACK_OK");
            }
        } else {
            erreurLog->enregistrerErreur("Erreur decodage Message APRS. Trame: " + incoming);
        }
    } 
    else {
        // CAS B : C'est une diffusion générale (La météo du collègue)
        // On vérifie que les balises météo t, h et b sont bien là
        if (incoming.indexOf('t') != -1 && incoming.indexOf('h') != -1 && incoming.indexOf('b') != -1) {
            decoderTrameWeather(incoming);
        } else {
            Serial.println("LOG: Trame generale recue (Non meteo).");
        }
    }
}