/**
 * @file Log.h
 * @brief Déclaration de la classe Log
 * @version 1.0
 * @author nbrands
 * @date 22/05/2026
 * @details Classe gérant l'enregistrement et l'affichage des erreurs du système de manière sécurisée.
 */

#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

class Log {
public:
    /**
     * @brief Enregistre et affiche un message d'erreur
     * @param msg Le message d'erreur à journaliser (pointeur vers une chaîne constante)
     * @return true si l'enregistrement a réussi, false si le message est nul ou vide
     */
    bool enregistrerErreur(const char* msg);
};

#endif