#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <cstring>
#include <cstdlib>
#include <cstddef>

#define MAX_CALLSIGN_LEN 10
#define MAX_DEST_LEN 10
#define MAX_PATH_LEN 20
#define MAX_RECIPIENT_LEN 10
#define MAX_COMMENT_LEN 68
#define MAX_PDU_LEN 150
#define MAX_MESSAGES_ID 99999

class Message {
public:
    Message(); 
    bool init(const char* _callsign, const char* _destination, const char* _path, const char* _recipient, const char* _comment);
    
    bool getPduMes(bool ackMode, char* outputBuffer, size_t bufferSize);
    int getPduLength();
    
    bool setComment(const char* _comment);
    bool setCallsign(const char* _callsign);
    bool setRecipient(const char* _recipient);
    
    bool decode(const char* trameRecue);

    bool getCallsign(char* outputBuffer, size_t size);
    bool getRecipient(char* outputBuffer, size_t size);
    bool getComment(char* outputBuffer, size_t size);
    bool isAckRequested();
    int getMessageId();
    
private:
    char    callsign[MAX_CALLSIGN_LEN];
    char    destination[MAX_DEST_LEN];
    char    path[MAX_PATH_LEN];
    char    recipient[MAX_RECIPIENT_LEN];
    char    comment[MAX_COMMENT_LEN];

    char    pdu[MAX_PDU_LEN];
    int     pduLength;
    int     messageId; 
    bool    ack;
};

#endif /* MESSAGE_H */