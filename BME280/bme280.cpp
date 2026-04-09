/*!
    \file     bme280.cpp
    \author   Philippe SIMIER (Touchard Wahington le Mans)
    \license  BSD (see license.txt)
    \brief    Classe pour le composant i2c  BME280
    \detail   Le BME280 est un capteur environnemental pour mesurer la température,
              la pression barométrique et l'humidité relative! Ce capteur est idéal
              pour réaliser une petite station météo. Il peut être connecté sur
              un bus I2C ou SPI! La broche CSB doit être connecté à VDDIO pour
              sélectionner l'interface I²C. Son adresse sur le bus est 0x77 ou 0x76
              en fonction du niveau de tension appliquée sur la broche SDO.
    \version    2.0 - First release decembre 2019
 *              2.1 - Second release mars 2024
 */

#include "bme280.h"
#include "i2c.h"

using namespace std;

// Le constructeur

BME280::BME280(int i2cAddress) {
    deviceI2C = new i2c(i2cAddress);
    if (!deviceI2C->getError()) {

        readCalibrationData();
        deviceI2C->WriteReg8(0xf2, 0x01); // humidity oversampling x 1

        deviceI2C->WriteReg8(0xf4, 0x27); // pressure and temperature oversampling x 1, mode normal
        h = -78.0; // différence d'altitude avec le niveau de la mer

    } else {

        throw std::runtime_error("Exception in constructor BME280");
    }
}

BME280::~BME280() {
    if (deviceI2C != NULL)
        delete deviceI2C;
}


// Méthode pour obtenir le Chip ID (0x60 pour le BME280)

unsigned int BME280::obtenirChipID() {
    return (unsigned int) deviceI2C->ReadReg8(CHIPID);
}

void BME280::readCalibrationData() {
    cal.dig_T1 = (uint16_t) deviceI2C->ReadReg16(DIG_T1);
    cal.dig_T2 = (int16_t) deviceI2C->ReadReg16(DIG_T2);
    cal.dig_T3 = (int16_t) deviceI2C->ReadReg16(DIG_T3);

    cal.dig_P1 = (uint16_t) deviceI2C->ReadReg16(DIG_P1);
    cal.dig_P2 = (int16_t) deviceI2C->ReadReg16(DIG_P2);
    cal.dig_P3 = (int16_t) deviceI2C->ReadReg16(DIG_P3);
    cal.dig_P4 = (int16_t) deviceI2C->ReadReg16(DIG_P4);
    cal.dig_P5 = (int16_t) deviceI2C->ReadReg16(DIG_P5);
    cal.dig_P6 = (int16_t) deviceI2C->ReadReg16(DIG_P6);
    cal.dig_P7 = (int16_t) deviceI2C->ReadReg16(DIG_P7);
    cal.dig_P8 = (int16_t) deviceI2C->ReadReg16(DIG_P8);
    cal.dig_P9 = (int16_t) deviceI2C->ReadReg16(DIG_P9);

    cal.dig_H1 = (uint8_t) deviceI2C->ReadReg8(DIG_H1);
    cal.dig_H2 = (int16_t) deviceI2C->ReadReg16(DIG_H2);
    cal.dig_H3 = (uint8_t) deviceI2C->ReadReg8(DIG_H3);
    cal.dig_H4 = (deviceI2C->ReadReg8(DIG_H4) << 4) | (deviceI2C->ReadReg8(DIG_H4 + 1) & 0xF);
    cal.dig_H5 = (deviceI2C->ReadReg8(DIG_H5 + 1) << 4) | (deviceI2C->ReadReg8(DIG_H5) >> 4);
    cal.dig_H6 = (int8_t) deviceI2C->ReadReg8(DIG_H6);
}

void BME280::getRawData() {

    deviceI2C->Write(0xf7);


    raw.pmsb = deviceI2C->Read();
    raw.plsb = deviceI2C->Read();
    raw.pxsb = deviceI2C->Read();

    raw.tmsb = deviceI2C->Read();
    raw.tlsb = deviceI2C->Read();
    raw.txsb = deviceI2C->Read();

    raw.hmsb = deviceI2C->Read();
    raw.hlsb = deviceI2C->Read();

    raw.temperature = 0;
    raw.temperature = (raw.temperature | raw.tmsb) << 8;
    raw.temperature = (raw.temperature | raw.tlsb) << 8;
    raw.temperature = (raw.temperature | raw.txsb) >> 4;

    raw.pressure = 0;
    raw.pressure = (raw.pressure | raw.pmsb) << 8;
    raw.pressure = (raw.pressure | raw.plsb) << 8;
    raw.pressure = (raw.pressure | raw.pxsb) >> 4;

    raw.humidity = 0;
    raw.humidity = (raw.humidity | raw.hmsb) << 8;
    raw.humidity = (raw.humidity | raw.hlsb);
}

double BME280::obtenirTemperatureEnC() {
    double var1;
    double var2;
    double temperature;
    double temperature_min = -40;
    double temperature_max = 85;

    getRawData();
    var1 = ((double) raw.temperature) / 16384.0 - ((double) cal.dig_T1) / 1024.0;
    var1 = var1 * ((double) cal.dig_T2);
    var2 = (((double) raw.temperature) / 131072.0 - ((double) cal.dig_T1) / 8192.0);
    var2 = (var2 * var2) * ((double) cal.dig_T3);
    cal.t_fine = (int32_t) (var1 + var2);
    temperature = (var1 + var2) / 5120.0;
    if (temperature < temperature_min) {
        temperature = temperature_min;
    } else if (temperature > temperature_max) {
        temperature = temperature_max;
    }

    return temperature;
}

double BME280::obtenirTemperatureEnF() {
    double output = obtenirTemperatureEnC();
    output = (output * 9) / 5 + 32;
    return output;
}


// retourne la pression en hPa
// Le capteur retourne la pression en Pa sur unsigned 32 bit integer avec le format Q24.8
// (24 bits pour la partie entière et 8 bits pour la partie fractionnaire).
// 24674867 represente 24674867/256 = 96386.2 Pa ou 24674867/25600 963.862 hPa

double BME280::obtenirPression() {
    double var1;
    double var2;
    double var3;
    double pressure;
    double pressure_min = 0.0;
    double pressure_max = 110000.0;

    getRawData();
    var1 = ((double) cal.t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double) cal.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double) cal.dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double) cal.dig_P4) * 65536.0);
    var3 = ((double) cal.dig_P3) * var1 * var1 / 524288.0;
    var1 = (var3 + ((double) cal.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double) cal.dig_P1);

    /* avoid exception caused by division by zero */
    if (var1) {
        pressure = 1048576.0 - (double) raw.pressure;
        pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
        var1 = ((double) cal.dig_P9) * pressure * pressure / 2147483648.0;
        var2 = pressure * ((double) cal.dig_P8) / 32768.0;
        pressure = pressure + (var1 + var2 + ((double) cal.dig_P7)) / 16.0;
        if (pressure < pressure_min) {
            pressure = pressure_min;
        } else if (pressure > pressure_max) {
            pressure = pressure_max;
        }
    } else /* Invalid case */ {
        pressure = pressure_min;
    }

    return pressure / 100.0;

}

// retourne le taux d'humidité relative en %

double BME280::obtenirHumidite() {
    double humidity;
    double humidity_min = 0.0;
    double humidity_max = 100.0;
    double var1;
    double var2;
    double var3;
    double var4;
    double var5;
    double var6;

    var1 = ((double) cal.t_fine) - 76800.0;
    var2 = (((double) cal.dig_H4) * 64.0 + (((double) cal.dig_H5) / 16384.0) * var1);
    var3 = raw.humidity - var2;
    var4 = ((double) cal.dig_H2) / 65536.0;
    var5 = (1.0 + (((double) cal.dig_H3) / 67108864.0) * var1);
    var6 = 1.0 + (((double) cal.dig_H6) / 67108864.0) * var1 * var5;
    var6 = var3 * var4 * (var5 * var6);
    humidity = var6 * (1.0 - ((double) cal.dig_H1) * var6 / 524288.0);
    if (humidity > humidity_max) {
        humidity = humidity_max;
    } else if (humidity < humidity_min) {
        humidity = humidity_min;
    }

    return humidity;
}

// retourne la pression avec réduction au niveau de la mer
// Selon l'atmosphère standard internationale (ISA) ou atmosphère normalisée
// (appelée aussi QNH en aviation) qui ne tient pas compte de la température réelle.

double BME280::obtenirPression0() {
    double P = obtenirPression();
    return P * (pow(1.0 - (0.0065 * h / (273.15 + 15)), 5.255));
}

// h = différence d'altitude du capteur avec P (mètres),
// négatif pour les élévations, positif pour les dépressions (la Mer Morte par exemple)

void BME280::donnerAltitude(double altitude) {
    this->h = altitude * -1.0;
}

// retourne la valeur du point de rosée

double BME280::obtenirPointDeRosee() {
    double ai = 7.45;
    double bi = 235.0;
    double z1, z2, z3, es, e, tau;

    double t = obtenirTemperatureEnC();
    double hum = obtenirHumidite();

    z1 = (ai * t) / (bi + t);
    es = 6.1 * exp(z1 * 2.3025851);
    e = es * hum / 100;
    z2 = e / 6.1;
    z3 = 0.434292289 * log(z2);
    tau = (235 * z3) / (7.45 - z3)*100;
    tau = floor(tau) / 100;

    return tau;
}


// retourne la version de la classe

void BME280::version() {

    cout << "\nBME280 PSR Version 2.0\n" << endl;

}
