#ifndef STATION_APP_H
#define STATION_APP_H

#include "RadioManager.h"
#include "AprsBuilder.h"

class StationApp {
private:
    RadioManager radio;
    AprsBuilder builder;

    void modeProduction();
    void testFormatage();
    void testTransmission();

public:
    StationApp();
    void demarrer();
};

#endif