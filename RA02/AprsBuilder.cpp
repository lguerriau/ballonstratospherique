#include "AprsBuilder.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <cassert>

using namespace std;

AprsBuilder::AprsBuilder(const string& callsign) : indicatif(callsign) {
    assert(!callsign.empty());
    assert(callsign.length() < 16);
}

string AprsBuilder::getAPRSTimestamp() const {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    assert(t != static_cast<time_t>(-1));
    
    tm* gmt = gmtime(&t);
    assert(gmt != nullptr);
    
    stringstream ss;
    // Format MMDDHHMM pour APRS
    ss << setfill('0') << setw(2) << gmt->tm_mon + 1
       << setw(2) << gmt->tm_mday
       << setw(2) << gmt->tm_hour
       << setw(2) << gmt->tm_min;
    return ss.str();
}

bool AprsBuilder::validerDonneesCapteurs(float tempF, float hum, float press) const {
    assert(!std::isnan(tempF));
    assert(!std::isnan(hum));
    
    // Verifications de securite pour eviter d'envoyer n'importe quoi
    if (tempF < -45.0 || tempF > 185.0) return false;
    if (hum < 0.0 || hum > 100.0) return false;
    if (press < 300.0 || press > 1150.0) return false;
    return true;
}

string AprsBuilder::buildTrame(float tempF, float hum, float press) const {
    assert(!indicatif.empty());
    assert(!std::isnan(press));

    if (!validerDonneesCapteurs(tempF, hum, press)) {
        return "ERREUR : Valeur(s) capteur(s) hors limites !";
    }

    stringstream aprs;
    // Format Weather Station : Indicatif>APRS,WIDE1-1:_Horodatage
    aprs << indicatif << ">APRS,WIDE1-1:_" << getAPRSTimestamp();
    
    // c...s...g... : Direction/Vitesse/Rafales du vent (non supportes ici)
    aprs << "c...s...g..."; 
    
    // tTTT : Temperature en Fahrenheit (3 chiffres)
    aprs << "t" << setfill('0') << setw(3) << (int)round(tempF);
    
    // hHH : Humidite (2 chiffres, 00 = 100%)
    int h_int = (int)round(hum);
    if (h_int >= 100) h_int = 0;
    aprs << "h" << setfill('0') << setw(2) << h_int;
    
    // bPPPPP : Pression en dixiemes de hPa (5 chiffres)
    aprs << "b" << setfill('0') << setw(5) << (int)round(press * 10.0);
    
    return aprs.str();
}