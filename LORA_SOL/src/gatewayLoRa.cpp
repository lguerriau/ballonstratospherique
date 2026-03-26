/* * File:   gatewayLoRa.cpp
 * Author: F4GOH - KF4GOH (Optimisé par ton Partenaire de Code)
 * Purpose: Gateway LoRa APRS avec filtrage, ACK multiples et télémétrie Qt.
 */

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "Message.h"

// --- CONFIGURATION SÉCURITÉ ---
const String AUTHORIZED_CALLSIGN = "F4KMN-9"; 
const String VALID_COMMAND = "RSSI/SNR";

// --- Configuration des Pins (Vérifie selon ton matériel) ---
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 26
#define LED 25 

// --- Paramètres Radio ---
#define FREQUENCY 433775000
#define SPREADING_FACTOR 12
#define SIGNAL_BANDWIDTH 125000
#define CODING_RATE_4 5
#define TX_POWER 20

// --- Gestion ACK et Timing ---
const unsigned long ACK_TIMEOUT = 5000;
bool waitForAck = false;
unsigned long lastSentTime = 0;
String lastSentMsgId = "";

// Instance du message
Message mes("F4KMN-9", "APLT00", "WIDE1-1", "F4KMN    ", "Hello");

// Prototypes (Note le = "ACK" par défaut ici)
void sendLoRa(char* msg, int length);
void receiveLoRa();
void sendAck(String msgId, String status = "ACK");

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.println(F("\n--- Gateway LoRa APRS Prête ---"));

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  if (!LoRa.begin(FREQUENCY)) {
    Serial.println(F("[ERREUR] LoRa non détecté !"));
    while (true);
  }

  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  LoRa.setSignalBandwidth(SIGNAL_BANDWIDTH);
  LoRa.setCodingRate4(CODING_RATE_4);
  LoRa.enableCrc();
  LoRa.setTxPower(TX_POWER);
}

void loop() {
  receiveLoRa(); // Écoute constante

  // Gestion du Timeout d'ACK
  if (waitForAck && (millis() - lastSentTime > ACK_TIMEOUT)) {
    Serial.println(">>> [TIMEOUT] Pas d'ACK reçu pour l'ID " + lastSentMsgId);
    waitForAck = false;
    digitalWrite(LED, LOW);
  }

  // Envoi depuis Qt (ou le terminal série)
  if (Serial.available() > 0) {
    char input = Serial.read();
    while(Serial.available() > 0) Serial.read(); // Vidage du buffer

    if (input == 'm' || input == 't') {
      digitalWrite(LED, HIGH);
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

// Fonction d'envoi LoRa brute
void sendLoRa(char* msg, int length) {
  LoRa.beginPacket();
  LoRa.write('<'); // Header APRS binaire
  LoRa.write(0xFF);
  LoRa.write(0x01);
  LoRa.write((const uint8_t *) msg, length);
  LoRa.endPacket();
}

// Fonction d'envoi d'Accusé de Réception (avec gestion d'erreurs)
void sendAck(String msgId, String status) {
  String ackMsg = status + "{" + msgId;
  char pduAck[ackMsg.length() + 1];
  ackMsg.toCharArray(pduAck, sizeof(pduAck));
  sendLoRa(pduAck, strlen(pduAck));
  
  Serial.print(F(">>> Réponse envoyée au demandeur : "));
  Serial.println(ackMsg);
}

// Fonction de réception principale (L'erreur d'accolades venait d'ici !)
void receiveLoRa() {
  int packetSize = LoRa.parsePacket();
  if (packetSize == 0) return;

  String incoming = "";
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  // Nettoyage de l'en-tête APRS binaire
  if (incoming.length() >= 3 && incoming.startsWith("<")) {
    incoming = incoming.substring(3);
  }

  // -------------------------------------------------------------
  // CAS 1 : On reçoit un Accusé de Réception (ACK) suite à notre envoi
  // -------------------------------------------------------------
  if (incoming.startsWith("ACK")) {
    int openBrace = incoming.indexOf('{');
    if (openBrace != -1) {
      String ackId = incoming.substring(openBrace + 1);
      
      if (waitForAck && ackId == lastSentMsgId) {
        if (incoming.startsWith("ACK{")) {
          // SUCCÈS : On envoie la télémétrie formatée pour l'interface Qt !
          Serial.print("RSSI:");
          Serial.print(LoRa.packetRssi());
          Serial.print("|SNR:");
          Serial.println(LoRa.packetSnr());
        } else {
          // ERREUR DISTANTE : (Ex: ACK_CALL{12)
          Serial.print(F(">>> [ERREUR DISTANTE] : "));
          Serial.println(incoming.substring(0, openBrace));
        }
        waitForAck = false;
        digitalWrite(LED, LOW);
      }
    }
    return; // Très important : on sort de la fonction si c'est un ACK
  }

  // -------------------------------------------------------------
  // CAS 2 : On reçoit une requête d'une autre station
  // -------------------------------------------------------------
  int arrowIndex = incoming.indexOf('>');
  int idIndex = incoming.lastIndexOf('{');
  int colonIndex = incoming.lastIndexOf(':');

  if (arrowIndex != -1 && idIndex != -1) {
    String sender = incoming.substring(0, arrowIndex);
    String command = incoming.substring(colonIndex + 1, idIndex);
    command.trim();
    String msgId = incoming.substring(idIndex + 1);

    // Vérification du Callsign
    if (!sender.startsWith(AUTHORIZED_CALLSIGN)) {
      sendAck(msgId, "ACK_CALL"); // Erreur d'expéditeur
      return;
    }

    // Vérification de la Commande
    if (command == VALID_COMMAND) {
      sendAck(msgId, "ACK"); // C'est valide, on répond "ACK"
    } else {
      sendAck(msgId, "ACK_CMD"); // Commande inconnue
    }
  }
}