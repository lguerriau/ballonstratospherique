/* 
 * File:   main.cpp
 * Author: philippe SIMIER (Lycée Touchard Le Mans)
 *
 * Created on 31 juillet 2023, 17:02
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <chrono>
#include "MPU6050.h"

using namespace std;



int main(int argc, char** argv) {

    
    try {
        MPU6050 mpu(0x69);
        
        mpu.setAccSensibility(FS_2G);
        mpu.calibrate();          // calibration du capteur
        mpu.setDLPFMode(DLPF_5);  // Filtrage passe bas 5Hz

        auto accM = mpu.getAccelM();

        cout << setfill('0') << fixed << setprecision(2) << accM << endl;
        // attente du début de la chute libre
        while (accM > 0.5){
           std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Attendre 10 ms
           accM = mpu.getAccelM();
        }
        for( int i = 0; i<400; i++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Attendre 10 ms

            cout << mpu.getAccelM() << endl;

        }

    } catch (const runtime_error &e) {

        cout << "Exception caught: " << e.what() << endl;
    }
    return 1;
}
