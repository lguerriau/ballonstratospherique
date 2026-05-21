#ifndef GESTIONLORA_H
#define GESTIONLORA_H

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "Message.h"
#include "Modele.h" 
#include "Log.h"    

class GestionLoRa {
public:
    // Règle 7 : Paramètres validés à l'intérieur
    GestionLoRa(Modele* bdd, Log* logger);

    // Règle 7 : La valeur de retour doit être vérifiée par l'appelant
    bool begin();   
    void process();
    bool sendLoRa(const char* msg, int length);

private:
    bool receiveLoRa();
    bool sendAck(int msgId, const char* status);
    void decoderTrameWeather(const char* trame); 

    // Règle 9 : Un seul niveau de déréférencement max (*)
    Modele* laBdd;
    Log* erreurLog;

    bool waitForAck;
    unsigned long lastSentTime;
    int lastSentMsgId; // Règle 3 : Remplacement de String par type primitif statique
    Message mes;

    // Règle 3 : Buffers de taille fixe alloués statiquement à la compilation
    char authorizedCallsign[8];
    char validCommand[10];
    
    static const unsigned long ACK_TIMEOUT = 5000;
    static const int pinLED = 25;
    static const int pinCS = 18;
    static const int pinRST = 14;
    static const int pinIRQ = 26;
};

#endif /* GESTIONLORA_H */