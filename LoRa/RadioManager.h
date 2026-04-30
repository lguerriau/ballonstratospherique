#ifndef RADIO_MANAGER_H
#define RADIO_MANAGER_H

#include <string>
#include "RadioLib.h"
#include "RadioLib/src/hal/RPi/PiHal.h"

class RadioManager {
private:
    PiHal* hal;
    Module* module;
    SX1278* lora;
    float frequence;
    float bandwidth;
    int spreadingFactor;

public:
    RadioManager(float freq, float bw, int sf);
    ~RadioManager();
    bool initialiser();
    int envoyer(const std::string& message);
};

#endif