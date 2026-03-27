/* 
 * File:   main.cpp
 * Author: philippe SIMIER
 *
 * Created on 27 mars 2024, 08:21
 *  
 */

#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <chrono>
#include "bme280.h"

using namespace std;

int main(int argc, char** argv) {

    try {
        BME280 capteur(0x77);


        cout << "Capteur BME 280 présent sur le bus I2C\r" << endl;
        

        while (1) {
            cout << " Température (C)  : " << fixed << setprecision(1) << capteur.obtenirTemperatureEnC() << " °C\r" << endl;
            cout << " Pression         : " << fixed << setprecision(1) << capteur.obtenirPression() << " hPa\r" << endl;
            cout << " Humidité         : " << fixed << setprecision(1) << capteur.obtenirHumidite() << " %\r" << endl;
            this_thread::sleep_for(chrono::seconds(5));

        }
    } catch (const runtime_error &e) {
        cout << "Exception caught: " << e.what() << endl;
    }



    return 0;
}

