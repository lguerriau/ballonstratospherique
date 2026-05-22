/*!
    \file     bme280.h
    \author   Philippe SIMIER (Touchard Washington le Mans)
    \license  BSD (see license.txt)
    \brief    Classe pour le composant i2c BME280
    \version  2.2
 */

#ifndef BME280_H_INCLUDED
#define BME280_H_INCLUDED

#include <iostream>
#include <iomanip>
#include "i2c.h"
#include <stdint.h>
#include <math.h>

#define ADRESSE_I2C_BME280     0x77

// Registres de calibration
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
#define BME280_RESET  0xB6
#define CAL26         0xE1
#define CONTROLHUMID  0xF2
#define CONTROL       0xF4
#define CONFIG        0xF5
#define PRESSUREDATA  0xF7
#define TEMPDATA      0xFA
#define HUMIDDATA     0xFD

#define MEAN_SEA_LEVEL_PRESSURE         1013

/**
 * @struct bme280_raw_data
 * @brief Structure stockant les données brutes lues depuis les registres du BME280.
 */
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

/**
 * @struct bme280_calib_data
 * @brief Structure contenant les coefficients de calibration usine du capteur.
 */
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

    int32_t t_fine; /*!< Valeur intermédiaire de température fine partagée pour compenser la pression et l'humidité */
} bme280_calib_data;

/**
 * @class BME280
 * @brief Gestion du capteur environnemental BME280 via le bus I2C.
 */
class BME280 {
public:
    /**
     * @brief Constructeur de la classe BME280.
     * @param i2cAddress Adresse I2C du capteur (0x77 par défaut ou 0x76).
     */
    BME280(int i2cAddress = ADRESSE_I2C_BME280);
    
    /**
     * @brief Destructeur de la classe BME280.
     */
    ~BME280();

    /**
     * @brief Récupère l'identifiant unique du composant (0x60 attendu pour un BME280).
     * @return Identifiant du circuit intégré.
     */
    unsigned int obtenirChipID();

    /**
     * @brief Lit et convertit la température courante en degrés Celsius.
     * @return Température compensée en °C (Plage théorique : -40.0 à 85.0°C).
     */
    double obtenirTemperatureEnC();

    /**
     * @brief Convertit la température courante en degrés Fahrenheit.
     * @return Température compensée en °F.
     */
    double obtenirTemperatureEnF();

    /**
     * @brief Lit et convertit la pression atmosphérique absolue.
     * @return Pression compensée en hectopascals (hPa).
     */
    double obtenirPression();

    /**
     * @brief Lit et convertit l'humidité relative du milieu.
     * @return Taux d'humidité relative en pourcentage (0.0 à 100.0%).
     */
    double obtenirHumidite();

    /**
     * @brief Calcule la pression ramenée au niveau de la mer (QNH) selon le modèle ISA.
     * @return Pression corrigée au niveau de la mer en hPa.
     */
    double obtenirPression0();

    /**
     * @brief Affiche la version actuelle du pilote dans la console standard.
     */
    void version();

private:
    i2c *deviceI2C;             /*!< Pointeur vers l'instance de gestion du bus I2C */
    bme280_calib_data cal;      /*!< Données de calibration lues au démarrage */
    bme280_raw_data raw;        /*!< Dernières données physiques brutes extraites du capteur */
    double h;                   /*!< Différence d'altitude par rapport au niveau de la mer en mètres */

    /**
     * @brief Extrait l'ensemble des coefficients de calibration stockés en ROM par Bosch.
     */
    void readCalibrationData();

    /**
     * @brief Interroge le capteur pour remplir la structure interne avec les dernières mesures brutes.
     */
    void getRawData();
};

#endif // BME280_H_INCLUDED