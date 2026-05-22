/**
 * @file Message.cpp
 * @brief Implémentation de la classe Message
 * @version 1.0
 * @author nbrands
 * @date 22/05/2026
 * @details Contient le code métier pour l'assemblage et l'analyse syntaxique des trames APRS.
 */

#include "Message.h"

/**
 * @brief Message::Message
 * @details À la création, l'ID est initialisé à 1 et tous les tampons statiques sont purgés par sécurité.
 */
Message::Message() {
    messageId = 1; 
    ack = false;
    pduLength = 0;
    memset(callsign, 0, MAX_CALLSIGN_LEN);
    memset(destination, 0, MAX_DEST_LEN);
    memset(path, 0, MAX_PATH_LEN);
    memset(recipient, 0, MAX_RECIPIENT_LEN);
    memset(comment, 0, MAX_COMMENT_LEN);
    memset(pdu, 0, MAX_PDU_LEN);
}

/**
 * @brief Message::init
 * @details Remplit de manière sécurisée (strncpy) les attributs internes après vérification de leurs limites.
 * @param _callsign L'indicatif émetteur
 * @param _dest La destination
 * @param _path Le chemin
 * @param _recip Le destinataire
 * @param _comment Le texte du message
 * @return true si valide, false si un paramètre est nul ou trop long
 */
bool Message::init(const char* _callsign, const char* _dest, const char* _path, const char* _recip, const char* _comment) {
    if (_callsign == nullptr || _dest == nullptr || _path == nullptr || _recip == nullptr) return false;
    if (strlen(_callsign) >= MAX_CALLSIGN_LEN || strlen(_dest) >= MAX_DEST_LEN) return false;

    strncpy(this->callsign, _callsign, MAX_CALLSIGN_LEN - 1);
    strncpy(this->destination, _dest, MAX_DEST_LEN - 1);
    strncpy(this->path, _path, MAX_PATH_LEN - 1);
    strncpy(this->recipient, _recip, MAX_RECIPIENT_LEN - 1);
    
    if (_comment != nullptr) {
        strncpy(this->comment, _comment, MAX_COMMENT_LEN - 1);
    }
    return true;
}

bool Message::setComment(const char* _comment) {
    if (_comment == nullptr) return false;
    size_t inLen = strlen(_comment);
    if (inLen == 0) return false; 
    
    strncpy(comment, _comment, MAX_COMMENT_LEN - 1);
    comment[MAX_COMMENT_LEN - 1] = '\0'; 
    return true;
}

bool Message::setCallsign(const char* _callsign) {
    if (_callsign == nullptr) return false;
    strncpy(callsign, _callsign, MAX_CALLSIGN_LEN - 1);
    callsign[MAX_CALLSIGN_LEN - 1] = '\0';
    return true;
}

bool Message::setRecipient(const char* _recipient) {
    if (_recipient == nullptr) return false;
    strncpy(recipient, _recipient, MAX_RECIPIENT_LEN - 1);
    recipient[MAX_RECIPIENT_LEN - 1] = '\0';
    return true;
}

/**
 * @brief Message::getPduMes
 * @details Concatène les informations dans le tampon PDU selon la norme APRS. 
 * Ajoute l'ID entre accolades si un ACK est demandé.
 * @param ackMode Demande d'accusé de réception
 * @param outputBuffer Tampon de destination
 * @param bufferSize Taille maximale du tampon de destination
 * @return true si la création a réussi
 */
bool Message::getPduMes(bool ackMode, char* outputBuffer, size_t bufferSize) {
    if (outputBuffer == nullptr) return false;
    if (bufferSize < MAX_PDU_LEN) return false;

    messageId++;
    if (messageId > MAX_MESSAGES_ID) {
        messageId = 1;
    }

    if (ackMode) {
        snprintf(pdu, sizeof(pdu), "%s>%s,%s::%s:%s{%d", 
                 callsign, destination, path, recipient, comment, messageId);
    } else {
        snprintf(pdu, sizeof(pdu), "%s>%s,%s::%s:%s", 
                 callsign, destination, path, recipient, comment);
    }

    pduLength = strlen(pdu);
    strncpy(outputBuffer, pdu, bufferSize);
    return true;
}

int Message::getMessageId() { return messageId; }
bool Message::isAckRequested() { return ack; }
int Message::getPduLength() { return pduLength; }

bool Message::getCallsign(char* outputBuffer, size_t size) {
    if (outputBuffer == nullptr || size <= strlen(callsign)) return false; 
    strncpy(outputBuffer, callsign, size);
    return true;
}

bool Message::getRecipient(char* outputBuffer, size_t size) {
    if (outputBuffer == nullptr || size <= strlen(recipient)) return false;
    strncpy(outputBuffer, recipient, size);
    return true;
}

bool Message::getComment(char* outputBuffer, size_t size) {
    if (outputBuffer == nullptr || size <= strlen(comment)) return false;
    strncpy(outputBuffer, comment, size);
    return true;
}

/**
 * @brief Message::decode
 * @details Analyse une trame APRS entrante en utilisant le pointeur standard C (strchr, strstr).
 * Extrait le destinataire, l'expéditeur et le commentaire.
 * @param trameRecue La chaîne brute réceptionnée
 * @return true si le format contient bien les balises '>', '::' et ':'
 */
bool Message::decode(const char* trameRecue) {
    if (trameRecue == nullptr || strlen(trameRecue) < 5) return false;

    const char* ptrSup = strchr(trameRecue, '>');
    if (ptrSup == nullptr) return false;

    const char* ptrDoublePoints = strstr(trameRecue, "::");
    if (ptrDoublePoints == nullptr) return false;

    const char* debutDest = ptrDoublePoints + 2;
    const char* ptrSimplePoint = strchr(debutDest, ':');
    if (ptrSimplePoint == nullptr) return false;

    size_t callsignLen = ptrSup - trameRecue;
    if (callsignLen >= MAX_CALLSIGN_LEN) callsignLen = MAX_CALLSIGN_LEN - 1;
    strncpy(callsign, trameRecue, callsignLen);
    callsign[callsignLen] = '\0';

    size_t destLen = ptrSimplePoint - debutDest;
    if (destLen >= MAX_RECIPIENT_LEN) destLen = MAX_RECIPIENT_LEN - 1;
    strncpy(recipient, debutDest, destLen);
    recipient[destLen] = '\0';

    const char* debutComment = ptrSimplePoint + 1;
    const char* ptrAccolade = strchr(debutComment, '{');

    if (ptrAccolade != nullptr) {
        ack = true;
        size_t commentLen = ptrAccolade - debutComment;
        if (commentLen >= MAX_COMMENT_LEN) commentLen = MAX_COMMENT_LEN - 1;
        strncpy(comment, debutComment, commentLen);
        comment[commentLen] = '\0';
        
        const char* idStr = ptrAccolade + 1;
        if (strlen(idStr) > 0) {
            messageId = atoi(idStr);
        } else {
            messageId = 0; 
        }
    } else {
        ack = false;
        strncpy(comment, debutComment, MAX_COMMENT_LEN - 1);
        comment[MAX_COMMENT_LEN - 1] = '\0';
    }

    return true;
}