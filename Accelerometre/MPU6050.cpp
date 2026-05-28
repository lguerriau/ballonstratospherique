/**
 * @file MPU6050.cpp
 * @brief Implémentation de la classe MPU6050
 * @author Philippe SIMIER (Refactorisation : [Ton Nom])
 */

#include "MPU6050.h"

using namespace std;

MPU6050::MPU6050() : deviceI2C(nullptr) {
}

MPU6050::~MPU6050() {
    // Plus besoin de "delete" car nous avons banni l'allocation dynamique !
}

void MPU6050::begin(int8_t address) {
    // RÈGLE NASA N°3 : Remplacement du "new" par une allocation statique locale.
    // L'objet I2C est créé en RAM une seule fois sans fragmenter le système.
    static i2c instanceI2C(address);
    deviceI2C = &instanceI2C;

    if (deviceI2C->getError()) {
        throw std::runtime_error("Erreur de bus I2C (Capteur absent)");
    }

    deviceI2C->WriteReg8(PWR_MGMT_1, 0x00);
    deviceI2C->WriteReg8(SAMPLRT_DIV, 0x00);

    unsigned char id = deviceI2C->ReadReg8(WHO_AM_I);
    if (id != 0x68) {
        throw std::runtime_error("Exception identification MPU5060");
    }

    setDLPFMode(MPU6050::DLPF_5);
    sensibilityAcc = deviceI2C->ReadReg8(ACCEL_CONFIG) & SENSIBILITY_MASK;
    sensibilityGyr = deviceI2C->ReadReg8(GYRO_CONFIG) & SENSIBILITY_MASK;
    
    // NOTE: Toutes les fonctions wiringPi (Interruptions) ont été supprimées ici !
}

float MPU6050::getAccelZ() {
    if (deviceI2C == nullptr || deviceI2C->getError()) return 1.0f; // Sécurité anti-crash

    data dataAccel;
    dataAccel.uCData[1] = deviceI2C->ReadReg8(ACCEL_ZOUT_H);
    dataAccel.uCData[0] = deviceI2C->ReadReg8(ACCEL_ZOUT_L);
    float val = 0.0f;

    switch (sensibilityAcc) {
        case FS_2G:  val = dataAccel.sData / 16384.0f; break;
        case FS_4G:  val = dataAccel.sData / 8192.0f;  break;
        case FS_8G:  val = dataAccel.sData / 4096.0f;  break;
        case FS_16G: val = dataAccel.sData / 2048.0f;  break;
    }
    return val;
}

float MPU6050::getAccelY() {
    if (deviceI2C == nullptr) return 0.0f;
    data dataAccel;
    dataAccel.uCData[1] = deviceI2C->ReadReg8(ACCEL_YOUT_H);
    dataAccel.uCData[0] = deviceI2C->ReadReg8(ACCEL_YOUT_L);
    float val = 0.0f;
    switch (sensibilityAcc) {
        case FS_2G: val = dataAccel.sData / 16384.0f; break;
        case FS_4G: val = dataAccel.sData / 8192.0f; break;
        case FS_8G: val = dataAccel.sData / 4096.0f; break;
        case FS_16G: val = dataAccel.sData / 2048.0f; break;
    }
    return val;
}

float MPU6050::getAccelX() {
    if (deviceI2C == nullptr) return 0.0f;
    data dataAccel;
    dataAccel.uCData[1] = deviceI2C->ReadReg8(ACCEL_XOUT_H);
    dataAccel.uCData[0] = deviceI2C->ReadReg8(ACCEL_XOUT_L);
    float val = 0.0f;
    switch (sensibilityAcc) {
        case FS_2G: val = dataAccel.sData / 16384.0f; break;
        case FS_4G: val = dataAccel.sData / 8192.0f; break;
        case FS_8G: val = dataAccel.sData / 4096.0f; break;
        case FS_16G: val = dataAccel.sData / 2048.0f; break;
    }
    return val;
}

float MPU6050::getAccelM() {
    auto accX = getAccelX();
    auto accY = getAccelY();
    auto accZ = getAccelZ();
    return sqrt(accZ * accZ + accX * accX + accY * accY);
}

void MPU6050::getMotion3(int16_t &ax, int16_t &ay, int16_t &az) {
    if (deviceI2C == nullptr) return;
    data dataAccel[3];
    dataAccel[0].uCData[1] = deviceI2C->ReadReg8(ACCEL_XOUT_H);
    dataAccel[0].uCData[0] = deviceI2C->ReadReg8(ACCEL_XOUT_L);
    dataAccel[1].uCData[1] = deviceI2C->ReadReg8(ACCEL_YOUT_H);
    dataAccel[1].uCData[0] = deviceI2C->ReadReg8(ACCEL_YOUT_L);
    dataAccel[2].uCData[1] = deviceI2C->ReadReg8(ACCEL_ZOUT_H);
    dataAccel[2].uCData[0] = deviceI2C->ReadReg8(ACCEL_ZOUT_L);
    ax = dataAccel[0].sData;
    ay = dataAccel[1].sData;
    az = dataAccel[2].sData;
}

float MPU6050::getTemperature() {
    if (deviceI2C == nullptr) return 0.0f;
    data temp;
    temp.uCData[1] = deviceI2C->ReadReg8(TEMP_OUT_H);
    temp.uCData[0] = deviceI2C->ReadReg8(TEMP_OUT_L);
    return (float) temp.sData / 340.0f + 36.53f;
}

float MPU6050::getRotationX() {
    if (deviceI2C == nullptr) return 0.0f;
    data dataGyro;
    dataGyro.uCData[1] = deviceI2C->ReadReg8(GYRO_XOUT_H);
    dataGyro.uCData[0] = deviceI2C->ReadReg8(GYRO_XOUT_L);
    float val = 0.0f;
    switch (sensibilityGyr) {
        case FS_250DPS:  val = (float) (dataGyro.sData * 250) / 32768; break;
        case FS_500DPS:  val = (float) (dataGyro.sData * 500) / 32768; break;
        case FS_1000DPS: val = (float) (dataGyro.sData * 1000) / 32768; break;
        case FS_2000DPS: val = (float) (dataGyro.sData * 2000) / 32768; break;
    }
    return val;
}

float MPU6050::getRotationY() {
    if (deviceI2C == nullptr) return 0.0f;
    data dataGyro;
    dataGyro.uCData[1] = deviceI2C->ReadReg8(GYRO_YOUT_H);
    dataGyro.uCData[0] = deviceI2C->ReadReg8(GYRO_YOUT_L);
    float val = 0.0f;
    switch (sensibilityGyr) {
        case FS_250DPS:  val = (float) (dataGyro.sData * 250) / 32768; break;
        case FS_500DPS:  val = (float) (dataGyro.sData * 500) / 32768; break;
        case FS_1000DPS: val = (float) (dataGyro.sData * 1000) / 32768; break;
        case FS_2000DPS: val = (float) (dataGyro.sData * 2000) / 32768; break;
    }
    return val;
}

float MPU6050::getRotationZ() {
    if (deviceI2C == nullptr) return 0.0f;
    data dataGyro;
    dataGyro.uCData[1] = deviceI2C->ReadReg8(GYRO_ZOUT_H);
    dataGyro.uCData[0] = deviceI2C->ReadReg8(GYRO_ZOUT_L);
    float val = 0.0f;
    switch (sensibilityGyr) {
        case FS_250DPS:  val = (float) (dataGyro.sData * 250) / 32768; break;
        case FS_500DPS:  val = (float) (dataGyro.sData * 500) / 32768; break;
        case FS_1000DPS: val = (float) (dataGyro.sData * 1000) / 32768; break;
        case FS_2000DPS: val = (float) (dataGyro.sData * 2000) / 32768; break;
    }
    return val;
}

void MPU6050::getGyro3(int16_t &gx, int16_t &gy, int16_t &gz) {
    if (deviceI2C == nullptr) return;
    data dataGyro[3];
    dataGyro[0].uCData[1] = deviceI2C->ReadReg8(GYRO_XOUT_H);
    dataGyro[0].uCData[0] = deviceI2C->ReadReg8(GYRO_XOUT_L);
    dataGyro[1].uCData[1] = deviceI2C->ReadReg8(GYRO_YOUT_H);
    dataGyro[1].uCData[0] = deviceI2C->ReadReg8(GYRO_YOUT_L);
    dataGyro[2].uCData[1] = deviceI2C->ReadReg8(GYRO_ZOUT_H);
    dataGyro[2].uCData[0] = deviceI2C->ReadReg8(GYRO_ZOUT_L);
    gx = dataGyro[0].sData;
    gy = dataGyro[1].sData;
    gz = dataGyro[2].sData;
}

void MPU6050::setAccSensibility(Sensibility range) {
    if (deviceI2C == nullptr) return;
    char val0 = deviceI2C->ReadReg8(ACCEL_CONFIG) & ~SENSIBILITY_MASK;
    deviceI2C->WriteReg8(ACCEL_CONFIG, val0 | range);
    sensibilityAcc = deviceI2C->ReadReg8(ACCEL_CONFIG) & SENSIBILITY_MASK;
}

void MPU6050::setGyroSensibility(MPU6050::Sensibility range) {
    if (deviceI2C == nullptr) return;
    char val0 = deviceI2C->ReadReg8(GYRO_CONFIG) & ~SENSIBILITY_MASK;
    deviceI2C->WriteReg8(GYRO_CONFIG, val0 | range);
    sensibilityGyr = deviceI2C->ReadReg8(GYRO_CONFIG) & SENSIBILITY_MASK;
}

void MPU6050::setDLPFMode(Dlpf dlpf) {
    if (deviceI2C == nullptr) return;
    char val0 = deviceI2C->ReadReg8(CONFIG) & 0xF8;
    deviceI2C->WriteReg8(CONFIG, val0 | dlpf);
}

void MPU6050::setAccelOffset(int16_t offsetX, int16_t offsetY, int16_t offsetZ) {
    if (deviceI2C == nullptr) return;
    data dataOffset;
    dataOffset.sData = offsetX;
    deviceI2C->WriteReg8(XA_OFFS_H, dataOffset.uCData[1]);
    deviceI2C->WriteReg8(XA_OFFS_L, dataOffset.uCData[0]);
    dataOffset.sData = offsetY;
    deviceI2C->WriteReg8(YA_OFFS_H, dataOffset.uCData[1]);
    deviceI2C->WriteReg8(YA_OFFS_L, dataOffset.uCData[0]);
    dataOffset.sData = offsetZ;
    deviceI2C->WriteReg8(ZA_OFFS_H, dataOffset.uCData[1]);
    deviceI2C->WriteReg8(ZA_OFFS_L, dataOffset.uCData[0]);
}

void MPU6050::setGyroOffset(int16_t offsetX, int16_t offsetY, int16_t offsetZ) {
    if (deviceI2C == nullptr) return;
    data dataOffset;
    dataOffset.sData = offsetX;
    deviceI2C->WriteReg8(XG_OFFS_H, dataOffset.uCData[1]);
    deviceI2C->WriteReg8(XG_OFFS_L, dataOffset.uCData[0]);
    dataOffset.sData = offsetY;
    deviceI2C->WriteReg8(YG_OFFS_H, dataOffset.uCData[1]);
    deviceI2C->WriteReg8(YG_OFFS_L, dataOffset.uCData[0]);
    dataOffset.sData = offsetZ;
    deviceI2C->WriteReg8(ZG_OFFS_H, dataOffset.uCData[1]);
    deviceI2C->WriteReg8(ZG_OFFS_L, dataOffset.uCData[0]);
}

void MPU6050::getAccelOffset(int16_t &offsetX, int16_t &offsetY, int16_t &offsetZ) {
    if (deviceI2C == nullptr) return;
    data dataAccel[3];
    dataAccel[0].uCData[1] = deviceI2C->ReadReg8(XA_OFFS_H);
    dataAccel[0].uCData[0] = deviceI2C->ReadReg8(XA_OFFS_L);
    dataAccel[1].uCData[1] = deviceI2C->ReadReg8(YA_OFFS_H);
    dataAccel[1].uCData[0] = deviceI2C->ReadReg8(YA_OFFS_L);
    dataAccel[2].uCData[1] = deviceI2C->ReadReg8(ZA_OFFS_H);
    dataAccel[2].uCData[0] = deviceI2C->ReadReg8(ZA_OFFS_L);
    offsetX = dataAccel[0].sData;
    offsetY = dataAccel[1].sData;
    offsetZ = dataAccel[2].sData;
}

void MPU6050::getGyroOffset(int16_t &offsetX, int16_t &offsetY, int16_t &offsetZ) {
    if (deviceI2C == nullptr) return;
    data dataGyro[3];
    dataGyro[0].uCData[1] = deviceI2C->ReadReg8(XG_OFFS_H);
    dataGyro[0].uCData[0] = deviceI2C->ReadReg8(XG_OFFS_L);
    dataGyro[1].uCData[1] = deviceI2C->ReadReg8(YG_OFFS_H);
    dataGyro[1].uCData[0] = deviceI2C->ReadReg8(YG_OFFS_L);
    dataGyro[2].uCData[1] = deviceI2C->ReadReg8(ZG_OFFS_H);
    dataGyro[2].uCData[0] = deviceI2C->ReadReg8(ZG_OFFS_L);
    offsetX = dataGyro[0].sData;
    offsetY = dataGyro[1].sData;
    offsetZ = dataGyro[2].sData;
}

void MPU6050::calibrateA() {
    if (deviceI2C == nullptr) return;
    int16_t ax, ay, az, ox, oy, oz;
    int ready, i = 0;

    setAccSensibility(MPU6050::FS_2G);
    setAccelOffset(0, 0, 0);

    meansensorsA(200, ax, ay, az);
    ox = -ax / 8;
    oy = -ay / 8;
    oz = (16384 - az) / 8;

    do {
        i++;
        if (i > 35) throw std::runtime_error("Exception calibrate acc MPU5060");
        ready = 0;
        setAccelOffset(ox, oy, oz);

        meansensorsA(100, ax, ay, az);

        if (ax > 8 || ax < -8) ox = ox - ax / 8; else ready++;
        if (ay > 8 || ay < -8) oy = oy - ay / 8; else ready++;
        if (((az - 16384) > 8) || ((az - 16384) < -8)) oz = oz - (az - 16384) / 8; else ready++;
    } while (ready < 3);
}

void MPU6050::calibrateG() {
    if (deviceI2C == nullptr) return;
    int16_t gx, gy, gz, ox, oy, oz;
    int ready[3] = {0, 0, 0};
    int i = 0;

    setGyroSensibility(MPU6050::FS_250DPS);
    setGyroOffset(0, 0, 0);

    meansensorsG(100, gx, gy, gz);
    ox = -gx / 4;
    oy = -gy / 4;
    oz = -gz / 4;

    do {
        i++;
        if (i > 35) throw std::runtime_error("Exception calibrate gyro MPU5060");

        setGyroOffset(ox, oy, oz);
        meansensorsG(100, gx, gy, gz);

        if ((gx != 0) && !ready[0]) ox = ox - gx / 8; else ready[0] = 1;
        if ((gy != 0) && !ready[1]) oy = oy - gy / 8; else ready[1] = 1;
        if ((gz != 0) && !ready[2]) oz = oz - gz / 8; else ready[2] = 1;
    } while (!ready[0] && !ready[1] && !ready[2]);
}

void MPU6050::meansensorsA(int nb, int16_t &mean_ax, int16_t &mean_ay, int16_t &mean_az) {
    long som_ax = 0, som_ay = 0, som_az = 0;
    int16_t ax, ay, az;

    for (int i = 0; i < nb; i++) {
        getMotion3(ax, ay, az);
        som_ax += ax;
        som_ay += ay;
        som_az += az;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    mean_ax = som_ax / nb;
    mean_ay = som_ay / nb;
    mean_az = som_az / nb;
}

void MPU6050::meansensorsG(int nb, int16_t &mean_gx, int16_t &mean_gy, int16_t &mean_gz) {
    long som_gx = 0, som_gy = 0, som_gz = 0;
    int16_t gx, gy, gz;

    for (int i = 0; i < nb; i++) {
        getGyro3(gx, gy, gz);
        som_gx += gx;
        som_gy += gy;
        som_gz += gz;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    mean_gx = som_gx / nb;
    mean_gy = som_gy / nb;
    mean_gz = som_gz / nb;
}

// Déclaration de l'instance globale attendue par le main.cpp
MPU6050 mpu;