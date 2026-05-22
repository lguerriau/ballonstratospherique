/* * File:   LM75.h
 * Author: philippe Simier Lycée Touchard
 *
 * Created on 25 juillet 2023, 15:55
 */

#ifndef LM75_H
#define LM75_H

#include "i2c.h"
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <unistd.h>

/**
 * @class LM75
 * @brief Classe de gestion du capteur de température LM75 via le bus I2C.
 */
class LM75 {
    
public:
    /**
     * @brief Constructeur de la classe LM75.
     * @param address Adresse I2C du capteur (0x48 par défaut).
     */
    LM75(int8_t address = 0x48);
    
    /**
     * @brief Constructeur de copie de la classe LM75.
     * @param orig Instance d'origine à copier.
     */
    LM75(const LM75& orig);
    
    /**
     * @brief Destructeur virtuel de la classe LM75.
     */
    virtual ~LM75();
    
    /**
     * @brief Récupère la température courante mesurée par le capteur.
     * @return float La valeur de la température convertie en degrés Celsius (°C).
     */
    float getTemperature();
    
private:
    i2c *deviceI2C;                   /*!< Pointeur vers l'instance de gestion du bus I2C */
    bool  presence;                   /*!< Indicateur d'état/présence du capteur sur le bus */
};

#endif /* LM75_H */