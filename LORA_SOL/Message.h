#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>

#define MAX_MESSAGES_ID  99999

class Message {
public:
    Message(String _callsign,String _destination,String _path,String _recipient,String _comment);
    Message(const Message& orig);
    virtual ~Message();
    
    char* getPduMes(bool ack);
    int getPduLength();
    void setComment(String _comment);
    void setCallsign(String _callsign);
    void setRecipient(String _recipient);
    
    // Nouvelle méthode pour décoder
    bool decode(String trameRecue);

    // Getters pour récupérer les informations décodées
    String getCallsign();
    String getRecipient();
    String getComment();
    bool isAckRequested();
    int getMessageId();
    
private:
    String  callsign;
    String  destination;
    String  path;
    String  recipient;
    String  comment;

    char    pdu[150];
    int     pduLength;
    int     messageId; 
    bool    ack; // <-- AJOUT : La variable manquante pour le décodage
};

#endif /* MESSAGE_H */