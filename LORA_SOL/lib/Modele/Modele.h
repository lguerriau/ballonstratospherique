#ifndef MODELE_H
#define MODELE_H

#include <Arduino.h>

class Modele {
public:
    bool enregistrerTelemetrie(const char* donnee);
};

#endif