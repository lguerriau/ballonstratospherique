#ifndef GESTIONLORA_H
#define GESTIONLORA_H

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "Message.h"
#include "Modele.h" // Ajout de la classe du collègue
#include "Log.h"    // Ajout de la classe du collègue

class GestionLoRa {
public:
    // Constructeur modifié pour accepter la BDD et le Log
    GestionLoRa(Modele* bdd, Log* logger);

    void begin();   
    void process();
    void sendLoRa(char* msg, int length);

private:
    void receiveLoRa();
    void sendAck(String msgId, String status = "ACK");
    void decoderTrameWeather(const String& trame); // Fonction du collègue intégrée

    // Pointeurs vers les outils du collègue
    Modele* laBdd;
    Log* erreurLog;

    bool waitForAck;
    unsigned long lastSentTime;
    String lastSentMsgId;
    Message mes;

    const String AUTHORIZED_CALLSIGN = "F4KMN";
    const String VALID_COMMAND = "RSSI/SNR";
    const unsigned long ACK_TIMEOUT = 5000;

    const int pinLED = 25;
    const int pinCS = 18;
    const int pinRST = 14;
    const int pinIRQ = 26;
};

#endif