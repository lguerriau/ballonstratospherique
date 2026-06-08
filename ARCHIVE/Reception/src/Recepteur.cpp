#include "Recepteur.h"

Recepteur::Recepteur(Modele* bdd, Log* log) : laBdd(bdd), erreurLog(log) {}

bool Recepteur::initialiser() {
    SPI.begin(pinSCK, pinMISO, pinMOSI, pinCS);
    LoRa.setPins(pinCS, pinRST, pinIRQ);

    if (!LoRa.begin(433775000)) { // Fréquence calée sur l'émetteur
        return false; 
    }

    LoRa.setSpreadingFactor(12); // SF12 comme sur la RPi
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.setSyncWord(0x12);
    LoRa.enableCrc();
    
    return true; 
}

bool Recepteur::verifierValiditeTrame(const String& trame) {
    return trame.indexOf("F4KMN") != -1; 
}

void Recepteur::decoderTrameWeather(const String& trame) {
    // Extraction de la température (t + 3 chiffres en Fahrenheit)
    int tPos = trame.indexOf('t');
    // Extraction de l'humidité (h + 2 chiffres)
    int hPos = trame.indexOf('h');
    // Extraction de la pression (b + 5 chiffres en dixièmes de hPa)
    int bPos = trame.indexOf('b');

    Serial.println("\n--- DÉCODAGE WEATHER ---");

    if (tPos != -1) {
        float tempF = trame.substring(tPos + 1, tPos + 4).toFloat();
        float tempC = (tempF - 32.0) * 5.0 / 9.0;
        Serial.print("Température : "); Serial.print(tempC, 1); Serial.println(" °C");
    }

    if (hPos != -1) {
        int hum = trame.substring(hPos + 1, hPos + 3).toInt();
        if (hum == 0) hum = 100; // Convention APRS
        Serial.print("Humidité    : "); Serial.print(hum); Serial.println(" %");
    }

    if (bPos != -1) {
        float press = trame.substring(bPos + 1, bPos + 6).toFloat() / 10.0;
        Serial.print("Pression    : "); Serial.print(press, 1); Serial.println(" hPa");
    }
    Serial.println("------------------------");
}

void Recepteur::ecouterEtRepondre() {
    int packetSize = LoRa.parsePacket();
    
    if (packetSize) {
        String trameRecue = "";
        while (LoRa.available()) {
            trameRecue += (char)LoRa.read();
        }

        // Si la trame commence par les 3 octets spéciaux, on les nettoie pour l'affichage
        String trameNettoyee = trameRecue;
        if (trameRecue.length() > 3 && trameRecue[0] == '<') {
            trameNettoyee = trameRecue.substring(3);
        }

        Serial.print("[RADIO] Reçu : ");
        Serial.println(trameNettoyee);

        if (verifierValiditeTrame(trameNettoyee)) {
            // 1. On enregistre dans la "base de données"
            laBdd->enregistrerTelemetrie(trameNettoyee);
            
            // 2. On décode et on affiche les valeurs météo
            decoderTrameWeather(trameNettoyee);

            // 3. Envoi de l'ACK
            LoRa.beginPacket();
            LoRa.print("ACK_OK");
            LoRa.endPacket();
        } else {
            Serial.println("[LOG] Trame reçue ignorée (Format non reconnu).");
        }
    }
}