/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Message.h
 * Author: ale
 *
 * Created on 5 décembre 2022, 18:18
 */

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
    
private:
    String  callsign;
    String  destination;
    String  path;
	String recipient;
	String comment;

    char    pdu[150];
    int     pduLength;
    int     messageId; 
    
};

#endif /* MESSAGE_H */

