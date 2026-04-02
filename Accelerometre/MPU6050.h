/* 
 * File:   MPU6050.h
 * Author: philippe SIMIER (Lycée Touchard Washington Le Mans)
 *
 * Created on 31 juillet 2023, 17:06
 */

#ifndef MPU6050_H
#define MPU6050_H

#include "i2c.h"
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <chrono>
#include <thread>
#include <sstream>
#include <wiringPi.h>

#define EARTH_GRAVITY_0 9.80665


#define    XA_OFFS_H  0x06 //[15:0] XA_OFFS
#define    XA_OFFS_L  0x07
#define    YA_OFFS_H  0x08 //[15:0] YA_OFFS
#define    YA_OFFS_L  0x09
#define    ZA_OFFS_H  0x0A //[15:0] ZA_OFFS
#define    ZA_OFFS_L  0x0B

#define    XG_OFFS_H  0x13
#define    XG_OFFS_L  0x14
#define    YG_OFFS_H  0x15
#define    YG_OFFS_L  0x16
#define    ZG_OFFS_H  0x17
#define    ZG_OFFS_L  0x18

#define    SAMPLRT_DIV 	0x19
#define    CONFIG       0x1A  // registre 26 Configuration du filtrage Passe Bas
#define    GYRO_CONFIG  0x1B  // registre 27 Gyroscope Configuration
#define    ACCEL_CONFIG 0x1C  // registre 28

#define    FF_THR       0x1D  // registre 29 Seuil Free Fall
#define    FF_DUR       0x1E  // registre 30 durée de Free Fall en ms
#define    MOT_THR      0x1F  // registre 31 seuil Motion
#define    MOT_DUR      0x20  // registre 32 durée de mvt en ms
#define    ZRMOT_THR    0x21  // registre 33 Seuil de zero mouvement
#define    ZRMOT_DUR    0x22  // registre 34 durée de zero mouvement
#define    FIFO_EN      0x23  // This register determines which sensor measurements are loaded into the FIFO buffer.

#define    INT_PIN_CONFIG 0x37
#define    INT_ENABLE     0x38
#define    INT_STATUS     0x3A

#define    ACCEL_XOUT_H  0x3B
#define    ACCEL_XOUT_L  0x3C
#define    ACCEL_YOUT_H  0x3D
#define    ACCEL_YOUT_L  0x3E
#define    ACCEL_ZOUT_H  0x3F
#define    ACCEL_ZOUT_L  0x40
#define    TEMP_OUT_H    0x41
#define    TEMP_OUT_L    0x42
#define    GYRO_XOUT_H   0x43
#define    GYRO_XOUT_L   0x44
#define    GYRO_YOUT_H   0x45
#define    GYRO_YOUT_L   0x46
#define    GYRO_ZOUT_H   0x47
#define    GYRO_ZOUT_L   0x48

#define    PWR_MGMT_1    0x6B
#define    WHO_AM_I      0x75

class MPU6050 {
public:

    // There are 4 sensibities for acceleration.
    // and 4 for full scale range of gyroscopes.
    enum Sensibility {
        FS_2G = 0x00, //xxx0 0xxx  calibre 0 : +/-2g
        FS_4G = 0x08, //xxx0 1xxx  calibre 1 : +/- 4g
        FS_8G = 0x10, //xxx1 0xxx  calibre 2 : +/- 8g
        FS_16G = 0x18, //xxx1 1xxx  calibre 3 : +/- 16g
        FS_250DPS = 0x00,
        FS_500DPS = 0x08,
        FS_1000DPS = 0x10,
        FS_2000DPS = 0x18,
        SENSIBILITY_MASK = 0x18 //0001 1000
    };

    enum Dlpf {
        DLPF_260, // 260Hz     | 0ms
        DLPF_184, // 184Hz     | 2.0ms
        DLPF_94, // 94Hz      | 3.0ms
        DLPF_44, // 44Hz      | 4.9ms
        DLPF_21, // 21Hz      | 8.5ms
        DLPF_10, // 10Hz      | 13.8ms
        DLPF_5 //  5Hz      | 19.0ms
    };

    MPU6050(void);
    virtual ~MPU6050(void);

    void  begin(int8_t address = 0x68);
    float getTemperature();
    float getAccelX();
    float getAccelY();
    float getAccelZ();
    float getAccelM();
    void  getMotion3(int16_t &ax, int16_t &ay, int16_t &az);
    
    float getRotationX();
    float getRotationY();
    float getRotationZ();
    void  getGyro3(int16_t &gx, int16_t &gy, int16_t &gz);

    void setAccSensibility(MPU6050::Sensibility range);
    void setGyroSensibility(MPU6050::Sensibility range);
    void setDLPFMode(MPU6050::Dlpf dlpf);

    void setAccelOffset(int16_t offsetX, int16_t offsetY, int16_t offsetZ);
    void getAccelOffset(int16_t &offsetX, int16_t &offsetY, int16_t &offsetZ);
    void setGyroOffset(int16_t offsetX, int16_t offsetY, int16_t offsetZ);
    void getGyroOffset(int16_t &offsetX, int16_t &offsetY, int16_t &offsetZ);

    void enableMotion(uint8_t thresold, uint8_t duration);
    void enableZeroMotion(uint8_t thresold, uint8_t duration);
    void enableFreeFall(uint8_t thresold, uint8_t duration);
    void disableInterupt();
    void onFreeFall(void (*userFunct)(void));
    void onMotion(void (*userFunct)(void));
    void onZeroMotion(void (*userFunct)(void));
    // Pointeurs des fonctions utilisateur
    void (*callback_FFD)(void);
    void (*callback_ZMD)(void);
    void (*callback_MD)(void);
    
    uint8_t getStatusInt();
  
    void calibrateA();
    void calibrateG();
    
    MPU6050::Sensibility sensibilityFromString(const std::string& str);
    MPU6050::Dlpf dlpfFromString(const std::string& str);

private:

    i2c *deviceI2C; // file descriptor
    int gpio_int; //raspberry GPIO pin connected to INT pin of MPU6050 chip
    char sensibilityAcc;
    char sensibilityGyr;
    
    union data {
        short sData;
        unsigned char uCData[2];
    };

    void meansensorsA(int nb, int16_t &mean_ax, int16_t &mean_ay, int16_t &mean_az);
    void meansensorsG(int nb, int16_t &mean_gx, int16_t &mean_gy, int16_t &mean_gz);
    static void interruptHandler();
    
 };


extern MPU6050 mpu;
#endif /* MPU6050_H */