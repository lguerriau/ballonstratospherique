/**
 * @file GestionLoRa.h
 * @brief Déclaration de la classe GestionLoRa
 * @version 1.0
 * @author nbrands
 * @date 22/05/2026
 * @details Classe gérant la communication matérielle avec la puce LoRa (SX1278).
 * Elle assure l'émission des commandes, la réception continue des trames, 
 * et l'aiguillage intelligent des données (Météo vers BDD, erreurs vers Log).
 * Code certifié selon les normes JPL/NASA.
 */

#ifndef GESTIONLORA_H
#define GESTIONLORA_H

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "Message.h"
#include "Modele.h" 
#include "Log.h"    

class GestionLoRa {
public:
    /**
     * @brief Constructeur de la passerelle LoRa
     * @param bdd Pointeur vers l'instance de la base de données (classe Modele)
     * @param logger Pointeur vers le gestionnaire de journalisation (classe Log)
     * @details Initialise les pointeurs et configure statiquement les paramètres de base.
     */
    GestionLoRa(Modele* bdd, Log* logger);

    /**
     * @brief Initialise le bus SPI et le module radio LoRa
     * @return true si l'initialisation a réussi, false en cas de panne matérielle ou de pointeur nul
     */
    bool begin();   
    
    /**
     * @brief Machine d'état principale (Chef d'orchestre)
     * @details Doit être appelée en boucle dans le loop(). 
     * Gère la lecture série, les timeouts et déclenche la réception radio.
     */
    void process();
    
    /**
     * @brief Envoie physiquement une trame sur les ondes radio
     * @param msg Pointeur vers la chaîne de caractères à émettre
     * @param length Longueur du message en octets
     * @return true si l'émission s'est déroulée avec succès
     */
    bool sendLoRa(const char* msg, int length);

private:
    /**
     * @brief Écoute et traite les trames radio entrantes
     * @return true si le traitement s'est bien passé (même si aucun paquet n'est reçu), false en cas d'erreur de sécurité
     */
    bool receiveLoRa();
    
    /**
     * @brief Forge et émet un accusé de réception (ACK)
     * @param msgId L'identifiant du message auquel on répond
     * @param status Le statut à renvoyer (ex: "ACK_OK")
     * @return true si l'acquittement a bien été envoyé
     */
    bool sendAck(int msgId, const char* status);
    
    /**
     * @brief Décode et affiche une trame météorologique générale
     * @param trame Pointeur vers la trame contenant les balises 't', 'h' et 'b'
     */
    void decoderTrameWeather(const char* trame); 

    /** Pointeur vers l'outil d'enregistrement en base de données */
    Modele* laBdd;
    /** Pointeur vers l'outil de gestion des erreurs */
    Log* erreurLog;

    /** Indicateur d'attente d'un accusé de réception */
    bool waitForAck;
    /** Chronomètre pour la gestion du Timeout de l'ACK */
    unsigned long lastSentTime;
    /** Mémorise l'ID du dernier message envoyé */
    int lastSentMsgId; 
    /** Instance statique gérant la création des trames APRS */
    Message mes;

    /** Buffer statique contenant l'indicatif autorisé */
    char authorizedCallsign[8];
    /** Buffer statique contenant la commande de demande de signal */
    char validCommand[10];
    
    /** Délai maximum d'attente d'un ACK en millisecondes */
    static const unsigned long ACK_TIMEOUT = 5000;
    
    /** Broche du microcontrôleur connectée à la LED de statut */
    static const int pinLED = 25;
    /** Broche Chip Select (CS) du bus SPI */
    static const int pinCS = 18;
    /** Broche de réinitialisation (RST) du module LoRa */
    static const int pinRST = 14;
    /** Broche d'interruption matérielle (DIO0) du module LoRa */
    static const int pinIRQ = 26;
};

#endif /* GESTIONLORA_H */