/*!
    \file     bme280.h
    \author   Philippe SIMIER (Touchard Wahington le Mans)
    \license  BSD (see license.txt)

    \brief    Classe pour le composant i2c  BME280
    \detail   Le BME280 est un capteur environnemental pour mesurer la température,
              la pression barométrique et l'humidité relative! Ce capteur est idéal
              pour réaliser une petite station météo. Il peut être connecté sur
              un bus I2C ou SPI! La broche CSB doit être connecté à VDDIO pour
              sélectionner l'interface I²C. Son adresse sur le bus est 0x77 ou 0x76
              en fonction du niveau de tension appliquée sur la broche SDO.

    \version    2.0 - Second release
 */



#ifndef BME280_H_INCLUDED
#define BME280_H_INCLUDED

#include <iostream>
#include <iomanip>
#include "i2c.h"
#include <stdint.h>       // pour les types uint8_t etc
#include <math.h>


#define ADRESSE_I2C_BME280     0x77
//Register names:

#define DIG_T1        0x88
#define DIG_T2        0x8A
#define DIG_T3        0x8C
#define DIG_P1        0x8E
#define DIG_P2        0x90
#define DIG_P3        0x92
#define DIG_P4        0x94
#define DIG_P5        0x96
#define DIG_P6        0x98
#define DIG_P7        0x9A
#define DIG_P8        0x9C
#define DIG_P9        0x9E
#define DIG_H1        0xA1
#define DIG_H2        0xE1
#define DIG_H3        0xE3
#define DIG_H4        0xE4
#define DIG_H5        0xE5
#define DIG_H6        0xE7
#define CHIPID        0xD0
#define VERSION       0xD1
#define SOFTRESET     0xE0
#define BME280_RESET                  0xB6
#define CAL26         0xE1
#define CONTROLHUMID  0xF2
#define CONTROL       0xF4
#define CONFIG        0xF5
#define PRESSUREDATA  0xF7
#define TEMPDATA      0xFA
#define HUMIDDATA     0xFD

#define MEAN_SEA_LEVEL_PRESSURE         1013


//  les registres des données pour la pression température et humidité

typedef struct {
    uint8_t pmsb;
    uint8_t plsb;
    uint8_t pxsb;

    uint8_t tmsb;
    uint8_t tlsb;
    uint8_t txsb;

    uint8_t hmsb;
    uint8_t hlsb;

    uint32_t temperature;
    uint32_t pressure;
    uint32_t humidity;

} bme280_raw_data;


// structure pour enregistrer les constantes  de calibration

typedef struct {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;

    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;

    int32_t t_fine;
} bme280_calib_data;

class BME280 {
public:
    // le constructeur
    BME280(int i2cAddress = ADRESSE_I2C_BME280);
    // le destructeur
    ~BME280();


    // Méthode pour obtenir le Chip ID (0x60 pour le BME280)
    unsigned int obtenirChipID();

    // méthodes pour lire la température le pression et l'humidité

    double obtenirTemperatureEnC();
    double obtenirTemperatureEnF();
    double obtenirPression();
    double obtenirHumidite();

    // méthode pour obtenir la pression au niveau de la mer
    void donnerAltitude(double h);
    double obtenirPression0();

    // méthode pour obtenir la valeur du point de rosée
    double obtenirPointDeRosee();

    // méthode pour obtenir la version
    void version();


private:


    i2c *deviceI2C; // file descriptor
    bme280_calib_data cal; // calibration
    bme280_raw_data raw; // les registres
    double h; // différence d'altitude du capteur avec le niveau de la mer en m

    void readCalibrationData();
    void getRawData();
};

#endif // BME280_H_INCLUDED