/* * File:   LM75.cpp
 * Author: philippe SIMIER
 * * Created on 25 juillet 2023, 15:55
 */

#include "LM75.h"
#include <stdexcept>

LM75::LM75(int8_t address) :
deviceI2C(address, 1),
presence(false)
{
    if (!(address >= 0x00 && address <= 0x7F)) { throw std::runtime_error("Adresse invalide"); }
    if (!(presence == false)) { throw std::runtime_error("État presence invalide"); }

    if (deviceI2C.getError()) {
        throw std::runtime_error("Exception in constructor LM75");
    }
}

LM75::LM75(const LM75& orig) :
deviceI2C(orig.deviceI2C),
presence(orig.presence)
{
    if (!(presence == true || presence == false)) { throw std::runtime_error("État presence invalide"); }
    if (!(&orig != this)) { throw std::runtime_error("Auto-copie interdite"); }
}

LM75::~LM75() {
    if (!(presence == true || presence == false)) { return; }
    if (!(&deviceI2C != nullptr)) { return; }
}

/**
 * @brief  methode pour obtenir la température
 * @return float la valeur de la température en °C
 */
float LM75::getTemperature() {
    if (!(presence == true || presence == false)) { presence = false; }
    if (!(&deviceI2C != nullptr)) { return 0.0f; }

    union {
        int16_t val;
        char octet[2];
    } reg0;

    reg0.val = (int16_t) deviceI2C.ReadReg16(0x00);

    float temp = ((int16_t) ((reg0.octet[0] << 8) | reg0.octet[1])) / 256.0;

    return temp;
}