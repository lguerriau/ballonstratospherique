/**
 * @file RadioManager.h
 * @brief Gestionnaire matériel de l'émetteur LoRa.
 */

#ifndef RADIO_MANAGER_H
#define RADIO_MANAGER_H

#include <string>

// Astuce de compatibilité : on définit _Bool pour le compilateur C++
#ifndef _Bool
#define _Bool bool
#endif

extern "C" {
    #include "LoRa.h"
}

/**
 * @class RadioManager
 * @brief Pilote l'interface matérielle radio LoRa via le bus SPI pour l'envoi de trames.
 */
class RadioManager {
private:
    LoRa_ctl modem;       /*!< Structure de contrôle bas niveau de la bibliothèque C LoRa */
    char txbuf[255];      /*!< Tampon d'envoi pour les données utiles (Payload) */

public:
    /**
     * @brief Constructeur par défaut. Initialise les structures mémoires.
     */
    RadioManager();
    
    /**
     * @brief Configure les broches GPIO, le SPI et les paramètres RF du module LoRa.
     * @return true si l'initialisation matérielle réussit, false sinon.
     */
    bool initialiser();
    
    /**
     * @brief Encapsule et transmet un message texte brut sur le canal LoRa configuré.
     * @param message Chaîne de caractères (trame APRS) à émettre.
     */
    void envoyer(const std::string& message);
};

#endif