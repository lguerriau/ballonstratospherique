/* 
 * File:   main.cpp
 * Author: philippe SIMIER
 *
 * Created on 25 juillet 2023, 15:54
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "LM75.h"
#include <unistd.h>

using namespace std;

int main(int argc, char** argv) {

    try {
        LM75 capteur;

        while (1) {
            cout << setprecision(3) << capteur.getTemperature() << "°C\r" <<endl;
            sleep(1);
        }
    } catch (const runtime_error &e) {
        cout << "Exception caught: " << e.what() << endl;
    }

    return 0;
}

