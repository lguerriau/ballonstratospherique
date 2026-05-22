/**
 * @file main.cpp
 * @brief Point d'entrée général du programme de la station météo APRS LoRa.
 */

#include "StationApp.h"

/**
 * @brief Fonction d'entrée principale. Instancie l'application globale et lance son cycle de vie.
 * @return 0 en cas d'arrêt attendu par l'utilisateur.
 */
int main() {
    StationApp app;
    app.demarrer();
    return 0;
}