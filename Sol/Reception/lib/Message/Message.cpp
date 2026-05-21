#include "Message.h"

Message::Message(String _callsign,String _destination,String _path,String _recipient,String _comment):
callsign(_callsign),
destination(_destination),
path(_path),
recipient(_recipient),
comment (_comment)
{
    messageId = 1; 
    ack = false; // Initialisation de sécurité
}

Message::Message(const Message& orig) {
}

Message::~Message() {
}

int Message::getPduLength() {
    return pduLength;
}

// L'ancienne fonction getMessageId() qui était en double ici a été supprimée !

void Message::setComment(String _comment) {
    comment=_comment;
    if (comment.length() > 67) {
        comment = comment.substring(0, 67); //67 char max
    }
}

void Message::setCallsign(String _callsign){
    callsign=_callsign;  
}

void Message::setRecipient(String _recipient){
    recipient=_recipient;
    recipient = recipient.substring(0, 9);
}

char* Message::getPduMes(bool _ackDemande){
    String header;
       
    header = callsign + ">" + destination + "," + path + "::"+recipient+":"+ comment;
    char messageArray[150];
    header.toCharArray(messageArray, 150);

    messageId++;
    if (messageId > MAX_MESSAGES_ID) {
        messageId = 0;
    }
    
    if (_ackDemande){
        snprintf(pdu, sizeof (pdu), "%s{%d", messageArray, messageId);
    }
    else {
        snprintf(pdu, sizeof (pdu), "%s", messageArray);	
    }		
    pduLength = strlen(pdu);
    return pdu;
}

// --- GETTERS ---
String Message::getCallsign() { return callsign; }
String Message::getRecipient() { return recipient; }
String Message::getComment() { return comment; }
bool Message::isAckRequested() { return ack; }
int Message::getMessageId() { return messageId; } // Unique définition ici

// --- METHODE DE DECODAGE ---
bool Message::decode(String trameRecue) {
    // 1. Trouver le Callsign (avant le caractère '>')
    int indexSup = trameRecue.indexOf('>');
    if (indexSup == -1) return false; 
    callsign = trameRecue.substring(0, indexSup);

    // 2. Trouver le début du Destinataire (après le '::')
    int indexDoublePoints = trameRecue.indexOf("::");
    if (indexDoublePoints == -1) return false;

    // 3. Trouver la fin du Destinataire (le ':' simple)
    int indexSimplePoint = trameRecue.indexOf(':', indexDoublePoints + 2);
    if (indexSimplePoint == -1) return false;
    
    // Extraction du Destinataire
    recipient = trameRecue.substring(indexDoublePoints + 2, indexSimplePoint);

    // 4. Extraire la fin de la trame
    String finDeTrame = trameRecue.substring(indexSimplePoint + 1);

    // 5. Vérifier s'il y a une demande d'accusé de réception
    int indexAccolade = finDeTrame.indexOf('{');
    
    if (indexAccolade != -1) {
        comment = finDeTrame.substring(0, indexAccolade);
        String idString = finDeTrame.substring(indexAccolade + 1);
        messageId = idString.toInt();
        ack = true;
    } else {
        comment = finDeTrame;
        ack = false;
    }

    return true; 
}