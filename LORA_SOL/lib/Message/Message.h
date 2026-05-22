/**
 * @file Message.h
 * @brief Déclaration de la classe Message
 * @version 1.0
 * @author nbrands
 * @date 22/05/2026
 * @details Classe modélisant le formatage et le décodage des trames radio APRS/LoRa.
 * Cette classe respecte les normes de codage statique (sans allocation dynamique).
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <cstring>
#include <cstdlib>
#include <cstddef>

/** Définition des bornes supérieures fixes pour les tableaux (Règle 2) */
#define MAX_CALLSIGN_LEN 10
#define MAX_DEST_LEN 10
#define MAX_PATH_LEN 20
#define MAX_RECIPIENT_LEN 10
#define MAX_COMMENT_LEN 68
#define MAX_PDU_LEN 150
#define MAX_MESSAGES_ID 99999

class Message {
public:
    /**
     * @brief Constructeur par défaut de la classe Message
     * @details Initialise les identifiants et met à zéro la mémoire des tampons.
     */
    Message(); 
    
    /**
     * @brief Initialise les paramètres du message
     * @param _callsign L'indicatif de l'émetteur
     * @param _destination Le code de destination APRS
     * @param _path Le chemin de répétition
     * @param _recipient Le destinataire du message
     * @param _comment Le contenu du message
     * @return true si l'initialisation a réussi, false en cas de dépassement de capacité
     */
    bool init(const char* _callsign, const char* _destination, const char* _path, const char* _recipient, const char* _comment);
    
    /**
     * @brief Génère la trame finale (PDU) prête à être envoyée
     * @param ackMode true si un accusé de réception est exigé
     * @param outputBuffer Le tableau de caractères où sera copiée la trame
     * @param bufferSize La taille allouée pour le tableau de sortie
     * @return true si la trame a été générée avec succès
     */
    bool getPduMes(bool ackMode, char* outputBuffer, size_t bufferSize);
    
    int getPduLength();
    
    bool setComment(const char* _comment);
    bool setCallsign(const char* _callsign);
    bool setRecipient(const char* _recipient);
    
    /**
     * @brief Décode une trame brute reçue par ondes radio
     * @param trameRecue Pointeur vers la chaîne de caractères reçue
     * @return true si le décodage respecte le format APRS attendu, false sinon
     */
    bool decode(const char* trameRecue);

    bool getCallsign(char* outputBuffer, size_t size);
    bool getRecipient(char* outputBuffer, size_t size);
    bool getComment(char* outputBuffer, size_t size);
    bool isAckRequested();
    int getMessageId();
    
private:
    /** Le callsign (indicatif) de la station émettrice */
    char    callsign[MAX_CALLSIGN_LEN];
    /** La destination APRS standardisée */
    char    destination[MAX_DEST_LEN];
    /** Le chemin de relais (ex: WIDE1-1) */
    char    path[MAX_PATH_LEN];
    /** L'indicatif de la station destinataire */
    char    recipient[MAX_RECIPIENT_LEN];
    /** Le contenu utile du message ou la commande */
    char    comment[MAX_COMMENT_LEN];

    /** Le tampon final contenant la trame assemblée */
    char    pdu[MAX_PDU_LEN];
    /** La longueur en octets de la trame finale */
    int     pduLength;
    /** L'identifiant unique du message pour la gestion des acquittements */
    int     messageId; 
    /** Vrai si l'émetteur attend un accusé de réception */
    bool    ack;
};

#endif /* MESSAGE_H */