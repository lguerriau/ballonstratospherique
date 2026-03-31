#include <Arduino.h>
#include "GestionLoRa.h"

GestionLoRa monLoRa;

void setup() {
    monLoRa.begin();
}

void loop() {
    monLoRa.process();
}