#ifndef GESTIONLORA_H
#define GESTIONLORA_H

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "Message.h"

class GestionLoRa {
public:
    // Constructeur
    GestionLoRa();

    // Méthodes principales
    void begin();   // Remplace le contenu du setup()
    void process(); // Remplace le contenu du loop()

private:
    // Méthodes internes (encapsulation)
    void sendLoRa(char* msg, int length);
    void receiveLoRa();
    void sendAck(String msgId, String status = "ACK");

    // Variables d'état
    bool waitForAck;
    unsigned long lastSentTime;
    String lastSentMsgId;

    // Instance de Message (utilisée en interne)
    Message mes;

    // Constantes de configuration
    const String AUTHORIZED_CALLSIGN = "F4KMN-9";
    const String VALID_COMMAND = "RSSI/SNR";
    const unsigned long ACK_TIMEOUT = 5000;

    // Pins
    const int pinLED = 25;
    const int pinCS = 18;
    const int pinRST = 14;
    const int pinIRQ = 26;
};

#endif