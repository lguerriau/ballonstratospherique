#include "GestionLoRa.h"

// Constructeur : Initialisation des variables par défaut
//"F4KMN-9" (Callsign) : C'est l'identifiant de ta station (ton indicatif radio). Le -9 est souvent utilisé pour désigner une station mobile (comme ton ballon).
//"APLT00" (Destination) : C'est un code standard APRS. Ici, il indique que les données sont envoyées via un système LoRa.
//"WIDE1-1" (Path) : C'est le "chemin" de répétition. Cela dit aux autres stations relais : "Si vous recevez ce message, répétez-le une fois".
//"F4KMN    " (Recipient) : C'est le destinataire par défaut. Note les espaces : la classe Message attend souvent un nom de 9 caractères. C'est la station au sol (ton PC/Qt) qui va filtrer ces messages.
//"Hello" (Comment) : C'est le texte par défaut qui sera envoyé si tu ne le modifies pas avec setComment().
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
    delay(100); // Petit temps de stabilisation
    Serial.println(F("\n--- Gateway LoRa APRS Prête ---"));

    // Initialisation SPI et LoRa
    SPI.begin(5, 19, 27, pinCS); // SCK, MISO, MOSI, CS
    LoRa.setPins(pinCS, pinRST, pinIRQ);

    if (!LoRa.begin(433775000)) {
        Serial.println(F("[ERREUR] LoRa non détecté !"));
        while (true);
    }

// --- Configuration avancée de la modulation LoRa ---

// Facteur d'étalement (SF) : 12 est le maximum. 
// Plus il est élevé, plus la portée est grande et le signal robuste, mais le débit est plus lent.
LoRa.setSpreadingFactor(12);

// Largeur de bande (BW) : 125 kHz (standard). 
// Une bande étroite augmente la sensibilité de réception au détriment de la vitesse.
LoRa.setSignalBandwidth(125000);

// Taux de codage (CR) : 4/5. 
// Ajoute des bits de correction d'erreurs (1 bit pour 4 bits de données) pour lutter contre les interférences.
LoRa.setCodingRate4(5);

// Activation du contrôle d'intégrité (CRC). 
// Permet de rejeter automatiquement les paquets de données corrompus pendant le vol.
LoRa.enableCrc();

// Puissance d'émission : 20 dBm (soit 100 mW). 
// C'est la puissance maximale autorisée pour ce module pour garantir une portée maximale vers le sol.
LoRa.setTxPower(20);
}

void GestionLoRa::process() {
    // 1. On écoute en permanence s'il y a des données qui arrivent de la Raspberry Pi (via RX/TX)
    if (Serial.available() > 0) {
        
        // On lit la ligne jusqu'au retour à la ligne envoyé par le serialPrintf de la Pi
        String inputFromPi = Serial.readStringUntil('\n');
        inputFromPi.trim(); // Nettoie les espaces ou caractères invisibles

        // Si on a bien reçu quelque chose (ex: "Z:1.02" ou "ST:BURST")
        if (inputFromPi.length() > 0) {
            digitalWrite(pinLED, HIGH);

            // 2. On injecte la donnée de la Pi dans le message APRS
            mes.setComment(inputFromPi);

            // 3. On génère la trame finale (PDU)
            // On met 'false' pour l'ACK : en vol, la nacelle n'attend pas de confirmation 
            // pour la télémétrie continue, elle "crache" ses données en aveugle.
            char* pdu = mes.getPduMes(false); 

            // (Optionnel) On renvoie l'info sur le port série pour le débogage
            // Attention : La Pi va recevoir cette ligne, assure-toi qu'elle ne la traite pas comme une erreur.
            // Serial.println("-> Transmission LoRa : " + String(pdu));

            // 4. Émission radio
            sendLoRa(pdu, mes.getPduLength());

            digitalWrite(pinLED, LOW);
        }
    }

    // On garde l'écoute LoRa au cas où le sol veuille envoyer une commande (ex: forcer une action)
    receiveLoRa(); 
    
    // Note : J'ai retiré la gestion du "waitForAck" ici pour la télémétrie courante, 
    // car on ne veut pas bloquer l'ESP32 s'il n'entend pas le sol.
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

    // Nettoyage de l'entête éventuelle
    if (incoming.length() >= 3 && incoming.startsWith("<")) {
        incoming = incoming.substring(3);
    }

    // --- CAS 1 : ACK (Accusé de réception) ---
    // Si la nacelle nous confirme qu'elle a bien reçu un ordre du sol
    if (incoming.startsWith("ACK")) {
        int openBrace = incoming.indexOf('{');
        if (openBrace != -1) {
            String ackId = incoming.substring(openBrace + 1);
            if (waitForAck && ackId == lastSentMsgId) {
                // On transmet les infos radio à Qt pour l'onglet "Transmissions"
                Serial.print("RSSI:");
                Serial.print(LoRa.packetRssi());
                Serial.print("|SNR:");
                Serial.println(LoRa.packetSnr());
                
                waitForAck = false;
                digitalWrite(pinLED, LOW);
            }
        }
        return;
    }

    // --- CAS 2 : Télémétrie entrante (Depuis la Nacelle) ---
    int arrowIndex = incoming.indexOf('>');
    int idIndex = incoming.lastIndexOf('{');     // Trouve la fin du message (avant l'ID)
    int colonIndex = incoming.lastIndexOf(':');  // Trouve le début du message utile

    // Si on a bien une trame APRS structurée
    if (arrowIndex != -1 && colonIndex != -1) {
        String sender = incoming.substring(0, arrowIndex);
        String command;
        
        // Extraction du message ("Z:1.02" ou "ST:BURST")
        if (idIndex != -1 && idIndex > colonIndex) {
            command = incoming.substring(colonIndex + 1, idIndex);
        } else {
            command = incoming.substring(colonIndex + 1);
        }
        command.trim();

        // Vérification de sécurité : est-ce bien NOTRE nacelle ?
        if (sender.startsWith(AUTHORIZED_CALLSIGN)) {
            
            // ---> LA LIGNE MAGIQUE POUR QT <---
            // On envoie le texte pur ("Z:1.02") dans le câble USB
            // La fonction lireDonneesSerie() de Qt va l'attraper instantanément !
            Serial.println(command);
            
            // Si c'est une commande spécifique qui exige une réponse, on envoie un ACK
            if (command == VALID_COMMAND) {
                sendAck(incoming.substring(idIndex + 1), "ACK");
            }
        }
    }
}