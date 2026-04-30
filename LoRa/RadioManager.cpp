#include "RadioManager.h"
#include <iostream>

using namespace std;

RadioManager::RadioManager(float freq, float bw, int sf) 
    : frequence(freq), bandwidth(bw), spreadingFactor(sf) {
    hal = new PiHal(0); 
    module = new Module(hal, 8, 4, 17, 18);
    lora = new SX1278(module); // CORRECTION DE L'ERREUR ICI
}

RadioManager::~RadioManager() {
    delete lora;
    delete module;
    delete hal;
}

bool RadioManager::initialiser() {
    int state = lora->begin(frequence, bandwidth, spreadingFactor, 5, 0x12);
    if (state != RADIOLIB_ERR_NONE) {
        cout << "Erreur init LoRa : " << state << endl;
        return false;
    }
    lora->setSyncWord(0x12);
    lora->setCRC(true);
    return true;
}

int RadioManager::envoyer(const string& message) {
    return lora->transmit(message.c_str());
}