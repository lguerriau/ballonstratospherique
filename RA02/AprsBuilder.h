#ifndef APRS_BUILDER_H
#define APRS_BUILDER_H

#include <string>

class AprsBuilder {
private:
    std::string indicatif;
    std::string getAPRSTimestamp() const;
    bool validerDonneesCapteurs(float tempF, float hum, float press) const;

public:
    AprsBuilder(const std::string& callsign);
    std::string buildTrame(float tempF, float hum, float press) const;
};

#endif