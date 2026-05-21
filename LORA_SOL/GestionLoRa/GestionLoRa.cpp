#include "GestionLoRa.h"

GestionLoRa::GestionLoRa(Modele* bdd, Log* logger) 
    : laBdd(bdd), erreurLog(logger),
      waitForAck(false), lastSentTime(0), lastSentMsgId(0) {
          
    // Règle 6 : Portée lexicale minimale et initialisation sécurisée
    strncpy(authorizedCallsign, "F4KMN", sizeof(authorizedCallsign) - 1);
    authorizedCallsign[sizeof(authorizedCallsign) - 1] = '\0';
    
    strncpy(validCommand, "RSSI/SNR", sizeof(validCommand) - 1);
    validCommand[sizeof(validCommand) - 1] = '\0';

    // Initialisation statique du message selon la NASA (Règle 3)
    bool initOk = mes.init("F4KMN-9", "APLT00", "WIDE1-1", "F4KMN    ", "Hello");
    
    // Règle 5 & 7 : Assertion et action correctrice si l'initialisation échoue
    if (!initOk && erreurLog != nullptr) {
        erreurLog->enregistrerErreur("ALERTE : Echec de l'initialisation du Message LoRa !");
    }
}

bool GestionLoRa::begin() {
    // Règle 5 (Assertion 1) : Validation du pointeur de dépendance BDD
    if (laBdd == nullptr) return false; 
    
    // Règle 5 (Assertion 2) : Validation du pointeur de dépendance Journal de Log
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
    
    return true; // Règle 7 : Statut renvoyé
}

void GestionLoRa::process() {
    // Règle 5 (Assertions) : Sécurité opérationnelle pré-calcul
    if (laBdd == nullptr || erreurLog == nullptr) return;

    // 1. Gestion des entrées du port Série
    if (Serial.available() > 0) {
        char input = Serial.read();
        
        // Règle 2 : Boucle de vidage avec borne supérieure fixe (max 64 itérations)
        for (int i = 0; (i < 64) && (Serial.available() > 0); i++) {
            (void)Serial.read(); // Règle 7 : Cast explicite pour ignorer la valeur
        }

        if (input == 'm') {
            mes.setComment(validCommand); 
            char pduAEnvoyer[150];
            memset(pduAEnvoyer, 0, sizeof(pduAEnvoyer));
            
            // Règle 7 : Chaque valeur de retour de fonction est vérifiée
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

    // 2. Gestion du Timeout d'attente d'ACK
    if (waitForAck && (millis() - lastSentTime > ACK_TIMEOUT)) { 
        erreurLog->enregistrerErreur("[TIMEOUT] Pas de reponse de la nacelle."); 
        waitForAck = false; 
        digitalWrite(pinLED, LOW); 
    }

    // 3. Écoute active et traitement
    bool resRx = receiveLoRa();
    (void)resRx; // Règle 7 : Acquittement explicite de la valeur de retour
}

bool GestionLoRa::sendLoRa(const char* msg, int length) {
    // Règle 5 (Assertion 1) : Pointeur valide
    if (msg == nullptr) return false;
    // Règle 5 (Assertion 2) : Hors limites de taille mémoire
    if (length <= 0 || length >= 150) return false;

    if (LoRa.beginPacket() == 0) return false;

    LoRa.write('<'); // KISS Header
    LoRa.write(0xFF);
    LoRa.write(0x01);
    
    size_t written = LoRa.write((const uint8_t *) msg, length);
    int endStatus = LoRa.endPacket();

    return (written == (size_t)length && endStatus == 1);
}

bool GestionLoRa::sendAck(int msgId, const char* status) {
    // Règle 5 (Assertions) : Validation stricte des données d'entrée
    if (msgId < 0 || status == nullptr) return false;

    char pduAck[150];
    memset(pduAck, 0, sizeof(pduAck));
    
    // Règle 8 : Utilisation sécurisée de snprintf
    snprintf(pduAck, sizeof(pduAck), "F4KMN-9>APLT00,WIDE1-1::NACELLE  :%s{%d", status, msgId);
    
    return sendLoRa(pduAck, strlen(pduAck));
}

void GestionLoRa::decoderTrameWeather(const char* trame) {
    // Règle 5 (Assertions) : Intégrité de la chaîne de caractères
    if (trame == nullptr || strlen(trame) == 0) return;

    // Règle 9 : Recherche via pointeurs sans double déréférencement
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
    // Règle 5 (Assertion 1) : Absence de paquet = comportement nominal
    if (packetSize == 0) return true;
    
    char incoming[150];
    memset(incoming, 0, sizeof(incoming));
    int count = 0;

    // Règle 2 & 5 (Assertion 2) : Lecture avec borne fixe (max 149 itérations)
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