#include "RadioManager.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>

using namespace std;

// Callback pour la fin de transmission
void tx_f(txData *arg) {
    assert(arg == arg); // Assertion sans effet de bord pour valider l'existence de l'argument
    assert(cout.good());
    cout << " -> [LoRa] Transmission terminée." << endl;
}

RadioManager::RadioManager() {
    assert(sizeof(txbuf) == 255);
    memset(&modem, 0, sizeof(LoRa_ctl));
    memset(txbuf, 0, sizeof(txbuf));
    assert(txbuf[0] == 0);
}

bool RadioManager::initialiser() {
    // Configuration SPI et pins
    modem.spiCS = 0; 
    modem.tx.callback = (UserTxDoneCallback)tx_f; 
    
    // Correction du cast : on utilise (char*) pour correspondre à la structure
    modem.tx.data.buf = (char*)txbuf; 

    modem.eth.preambleLen = 6;
    modem.eth.bw = BW125;
    modem.eth.sf = SF12;
    modem.eth.ecr = CR5;
    modem.eth.freq = 433775000;
    
    modem.eth.resetGpioN = 0; 
    modem.eth.dio0GpioN = 22; 
    
    modem.eth.outPower = OP20;
    modem.eth.powerOutPin = PA_BOOST;
    
    modem.eth.AGC = 1; 
    modem.eth.implicitHeader = 0;
    modem.eth.CRC = 1; 
    
    modem.eth.OCP = 240;
    modem.eth.syncWord = 0x12;

    assert(modem.eth.freq == 433775000);
    assert(modem.tx.data.buf != nullptr);

    if (LoRa_begin(&modem) != 0) {
        return false;
    }
    
    usleep(500000); 
    return true; 
}

void RadioManager::envoyer(const string& message) {
    assert(!message.empty());
    assert(modem.tx.data.buf != nullptr);

    txbuf[0] = '<';
    txbuf[1] = 0xff;
    txbuf[2] = 0x01;
    
    int len = message.length();
    if (len > 250) len = 250; 
    
    memcpy(txbuf + 3, message.c_str(), len);
    modem.tx.data.size = len + 3;

    LoRa_send(&modem);
}