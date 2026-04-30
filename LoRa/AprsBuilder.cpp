#include "AprsBuilder.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>

using namespace std;

AprsBuilder::AprsBuilder(const string& callsign) : indicatif(callsign) {}

string AprsBuilder::getAPRSTimestamp() const {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm* gmt = gmtime(&t);
    stringstream ss;
    ss << setfill('0') << setw(2) << gmt->tm_mon + 1
       << setw(2) << gmt->tm_mday
       << setw(2) << gmt->tm_hour
       << setw(2) << gmt->tm_min;
    return ss.str();
}

bool AprsBuilder::validerDonneesCapteurs(float tempF, float hum, float press) const {
    if (tempF < -45.0 || tempF > 185.0) return false;
    if (hum < 0.0 || hum > 100.0) return false;
    if (press < 300.0 || press > 1100.0) return false;
    return true;
}

string AprsBuilder::buildTrame(float tempF, float hum, float press) const {
    if (!validerDonneesCapteurs(tempF, hum, press)) {
        std::cout << "[DEBUG] Valeurs reçues - Temp: " << tempF 
                  << "F, Hum: " << hum 
                  << "%, Press: " << press << " hPa" << std::endl;
        
        return "ERREUR : Valeur(s) capteur(s) hors limites !";
    }

    stringstream aprs;
    aprs << indicatif << ">APRS,WIDE1-1:_" << getAPRSTimestamp();
    aprs << "c...s...g..."; 
    aprs << "t" << setfill('0') << setw(3) << (int)round(tempF);
    
    int h_int = (int)round(hum);
    aprs << "h" << setfill('0') << setw(2) << (h_int >= 100 ? 0 : h_int);
    aprs << "b" << setfill('0') << setw(5) << (int)round(press * 10.0);
    
    return aprs.str();
}