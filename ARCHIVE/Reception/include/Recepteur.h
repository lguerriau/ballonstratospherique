#ifndef RECEPTEUR_H
#define RECEPTEUR_H

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "Modele.h"
#include "Log.h"

class Recepteur {
private:
    Modele* laBdd;
    Log* erreurLog;

    const int pinSCK  = 5;
    const int pinMISO = 19;
    const int pinMOSI = 27;
    const int pinCS   = 18;
    const int pinRST  = 14;
    const int pinIRQ  = 26;

    bool verifierValiditeTrame(const String& trame);
    // Nouvelle fonction de décodage interne
    void decoderTrameWeather(const String& trame); 

public:
    Recepteur(Modele* bdd, Log* log);
    bool initialiser();
    void ecouterEtRepondre();
};

#endif