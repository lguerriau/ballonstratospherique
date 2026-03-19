/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Message.cpp
 * Author: ale
 * 
 * Created on 5 décembre 2022, 18:18
 */

#include "Message.h"

Message::   Message(String _callsign,String _destination,String _path,String _recipient,String _comment):
callsign(_callsign),
destination(_destination),
path(_path),
recipient(_recipient),
comment (_comment)
{
messageId=1; 
}

Message::Message(const Message& orig) {
}

Message::~Message() {
 
}

int Message::getPduLength() {
    return pduLength;
}


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

char* Message::getPduMes(bool ack){
    String header;
       
   
    header = callsign + ">" + destination + "," + path + "::"+recipient+":"+ comment;
    char messageArray[150];
    header.toCharArray(messageArray, 150);

    messageId++;
    if (messageId > MAX_MESSAGES_ID) {
        messageId = 0;
    }
	if (ack){
    snprintf(pdu, sizeof (pdu), "%s{%d", messageArray, messageId);
	}
	else
	{
	snprintf(pdu, sizeof (pdu), "%s", messageArray);	
	}		
    pduLength = strlen(pdu);
    return pdu;
}
