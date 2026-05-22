/**
 * @file Modele.h
 * @brief Déclaration de la classe Modele
 * @version 1.0
 * @author nbrands
 * @date 22/05/2026
 * @details Classe gérant l'interfaçage avec la base de données.
 * Elle permet d'enregistrer les trames télémétriques reçues depuis la nacelle.
 */

#ifndef MODELE_H
#define MODELE_H

#include <Arduino.h>

class Modele {
public:
    /**
     * @brief Enregistre une trame de télémétrie dans la base de données
     * @param donnee Pointeur vers la chaîne de caractères de la trame reçue
     * @return true si l'enregistrement s'est bien déroulé, false si la donnée est vide ou nulle
     */
    bool enregistrerTelemetrie(const char* donnee);
};

#endif