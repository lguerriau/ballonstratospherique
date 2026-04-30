#ifndef RECEPTEUR_H
#define RECEPTEUR_H

#include <Arduino.h>
#include <RadioLib.h>
#include "Modele.h"
#include "Log.h"

class Recepteur {
private:
    SX1278* radio; // On utilise un pointeur vers la radio globale
    Modele* laBdd;
    Log* erreurLog;

    bool verifierValiditeTrame(const String& trame);
    bool verifierPresenceDonnee(const String& trame);

public:
    // Le constructeur prend désormais la radio en paramètre
    Recepteur(SX1278* r, Modele* bdd, Log* log);
    
    // Retourne un entier (le code d'erreur) au lieu d'un booléen
    int initialiser(float freq, float bw, int sf);
    
    void ecouterEtTraiter();
};

#endif