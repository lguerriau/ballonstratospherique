#ifndef RADIO_MANAGER_H
#define RADIO_MANAGER_H

#include <string>

// Astuce de compatibilité : on définit _Bool pour le compilateur C++
#ifndef _Bool
#define _Bool bool
#endif

extern "C" {
    #include "LoRa.h"
}

class RadioManager {
private:
    LoRa_ctl modem;
    char txbuf[255];

public:
    RadioManager();
    bool initialiser();
    void envoyer(const std::string& message);
};

#endif