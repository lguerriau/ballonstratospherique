/* 
 * File:   main.cpp
 * Author: F4GOH - KF4GOH
  * Created on 6 avril 2022, 13:14
 * todo :
 * improve parser
 * check wifi reconnection
 * 
 * * LoRa
 * ====
 * #ID: 1167
 * An Arduino library for sending and receiving data using LoRa radios. Supports Semtech SX1276/77/78/79 based boards/shields.
 * pio lib install 1167
 * Library Manager: LoRa @ 0.8.0 has been installed!
 *  
 * 
 * Bibliothèque : Oled 
 * installation : pio lib install 2978
 * 
 *  
 */

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "Message.h"


#define LED 25 //green led 

#define FREQUENCY 433775000
#define SPREADING_FACTOR 12
#define SIGNAL_BANDWIDTH 125000
#define CODING_RATE_4 5
#define TX_POWER 20
#define MSG_MAX_LENGTH 100


int counter = 0;

Message mes("F4KMN-9", "APLT00", "WIDE1-1", "F4KMN    ", "Hello");

void send(char* msg,int length);
void receive();


void setup() {
  Serial.begin(115200);
  while (!Serial);

    Serial.println("Set SPI pins!");
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
 
  Serial.println("Set LoRa pins!");
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  
  Serial.println("frequency: ");
  Serial.println(String(FREQUENCY));
  if (!LoRa.begin(FREQUENCY)) {
    Serial.println("Starting LoRa failed!");    
    while (true) {
    }
  }
  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  LoRa.setSignalBandwidth(SIGNAL_BANDWIDTH);
  LoRa.setCodingRate4(CODING_RATE_4);
  LoRa.enableCrc();
  
  LoRa.setTxPower(TX_POWER);//Config.lora.power);
  Serial.println("LoRa init done!");
  
  
  
  
}

void loop() {
   char c;
  char *pdu;
  receive();
  if (Serial.available() > 0) {
    c = Serial.read();
    digitalWrite(LED, HIGH);
    switch (c) {
      case 'm':
        Serial.println(F("Send a message"));
        mes.setRecipient("F4KMN    ");
        mes.setComment("RSSI/SNR");
        pdu = mes.getPduMes(false);
        Serial.println(pdu);
        send(pdu, mes.getPduLength());
        break;
    }
    digitalWrite(LED, LOW);
  }
}

  void send(char* msg,int length){
    LoRa.beginPacket();
    // Header:
    LoRa.write('<');
    LoRa.write(0xFF);
    LoRa.write(0x01);
    // APRS Data:
    LoRa.write((const uint8_t *) msg,length);
    LoRa.endPacket();    
}
 
  void receive() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    // print SNR of packet
    Serial.println(LoRa.packetSnr());
        Serial.print("' with SNR ");
  }
}