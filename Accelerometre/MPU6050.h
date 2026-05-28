/**
 * @file MPU6050.h
 * @brief Déclaration de la classe MPU6050
 * @author Philippe SIMIER (Refactorisation : [Ton Nom])
 * @version 2.0 (Norme NASA JPL)
 * @details Pilote matériel I2C de l'accéléromètre. Purgé des interruptions 
 * matérielles (wiringPi) et de l'allocation dynamique pour une stabilité absolue.
 */

#ifndef MPU6050_H
#define MPU6050_H

#include "i2c.h"
#include <stdint.h>
#include <iostream>
#include <math.h>
#include <chrono>
#include <thread>
#include <stdexcept>

// --- Définition des adresses de registres I2C (Conservées) ---
#define XA_OFFS_H  0x06 
#define XA_OFFS_L  0x07
#define YA_OFFS_H  0x08 
#define YA_OFFS_L  0x09
#define ZA_OFFS_H  0x0A 
#define ZA_OFFS_L  0x0B
#define XG_OFFS_H  0x13
#define XG_OFFS_L  0x14
#define YG_OFFS_H  0x15
#define YG_OFFS_L  0x16
#define ZG_OFFS_H  0x17
#define ZG_OFFS_L  0x18
#define SAMPLRT_DIV  0x19
#define CONFIG       0x1A 
#define GYRO_CONFIG  0x1B 
#define ACCEL_CONFIG 0x1C 
#define ACCEL_XOUT_H  0x3B
#define ACCEL_XOUT_L  0x3C
#define ACCEL_YOUT_H  0x3D
#define ACCEL_YOUT_L  0x3E
#define ACCEL_ZOUT_H  0x3F
#define ACCEL_ZOUT_L  0x40
#define TEMP_OUT_H    0x41
#define TEMP_OUT_L    0x42
#define GYRO_XOUT_H   0x43
#define GYRO_XOUT_L   0x44
#define GYRO_YOUT_H   0x45
#define GYRO_YOUT_L   0x46
#define GYRO_ZOUT_H   0x47
#define GYRO_ZOUT_L   0x48
#define PWR_MGMT_1    0x6B
#define WHO_AM_I      0x75

class MPU6050 {
public:
    enum Sensibility {
        FS_2G = 0x00, 
        FS_4G = 0x08, 
        FS_8G = 0x10, 
        FS_16G = 0x18, 
        FS_250DPS = 0x00,
        FS_500DPS = 0x08,
        FS_1000DPS = 0x10,
        FS_2000DPS = 0x18,
        SENSIBILITY_MASK = 0x18 
    };

    enum Dlpf {
        DLPF_260, DLPF_184, DLPF_94, DLPF_44, DLPF_21, DLPF_10, DLPF_5 
    };

    MPU6050(void);
    virtual ~MPU6050(void);

    /**
     * @brief Initialise le capteur sur le bus I2C
     * @throws std::runtime_error si le capteur est débranché (Active le mode simulation du main.cpp)
     */
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

    void calibrateA();
    void calibrateG();

private:
    i2c *deviceI2C; // Pointeur sécurisé géré sans allocation dynamique
    char sensibilityAcc;
    char sensibilityGyr;
    
    /** @brief Union brillante pour fusionner les octets I2C sans calculs lourds */
    union data {
        short sData;
        unsigned char uCData[2];
    };

    void meansensorsA(int nb, int16_t &mean_ax, int16_t &mean_ay, int16_t &mean_az);
    void meansensorsG(int nb, int16_t &mean_gx, int16_t &mean_gy, int16_t &mean_gz);
};

// Variable globale requise par le main.cpp
extern MPU6050 mpu;

#endif /* MPU6050_H */