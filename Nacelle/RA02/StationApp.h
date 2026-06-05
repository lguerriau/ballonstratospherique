/**
 * @file StationApp.h
 * @brief Automate et interface utilisateur de la station météo.
 */

#ifndef STATION_APP_H
#define STATION_APP_H

#include "RadioManager.h"
#include "AprsBuilder.h"

/**
 * @class StationApp
 * @brief Classe maîtresse gérant les modes de fonctionnement (production/tests) et l'interface menu.
 */
class StationApp {
private:
    RadioManager radio;    /*!< Instance du contrôleur d'émission radio */
    AprsBuilder builder;   /*!< Instance de formatage du protocole APRS */

    /**
     * @brief Boucle d'acquisition automatique continue des capteurs physiques et envoi radio périodique.
     */
    void modeProduction();

    /**
     * @brief Lit et affiche en continu la température du capteur LM75 seul.
     * @details Permet de tester individuellement le composant LM75 sur le bus I2C sans l'infrastructure APRS/LoRa.
     */
    void lireCapteurLM75();

    /**
     * @brief Outil interactif permettant de simuler des saisies météo et d'observer la trame résultante.
     */
    void testFormatage();

    /**
     * @brief Force l'émission LoRa d'une trame d'essai générée manuellement.
     */
    void testTransmission();

public:
    /**
     * @brief Prépare l'application et définit l'indicatif par défaut.
     */
    StationApp();
    
    /**
     * @brief Lance l'affichage du menu interactif persistant sur le terminal.
     */
    void demarrer();
};

#endif