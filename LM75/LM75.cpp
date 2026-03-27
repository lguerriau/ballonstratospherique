/* 
 * File:   LM75.cpp
 * Author: philippe SIMIER
 * 
 * Created on 25 juillet 2023, 15:55
 */

#include "LM75.h"

LM75::LM75(int8_t address) :
deviceI2C(new i2c(address)),
presence(false)
{
    if (deviceI2C->getError()) {

        throw std::runtime_error("Exception in constructor LM75");
    }

}

LM75::LM75(const LM75& orig) {
}

LM75::~LM75() {
    if (deviceI2C != nullptr)
        delete deviceI2C;
}

/**
 * @brief  methode pour obtenir la température
 * @return float la valeur de la température en °C
 */
float LM75::getTemperature() {

    union {
        int16_t val;
        char octet[2];
    } reg0;

    reg0.val = (int16_t) deviceI2C->ReadReg16(0x00);

    float temp = ((int16_t) ((reg0.octet[0] << 8) | reg0.octet[1])) / 256.0;

    return temp;

}
