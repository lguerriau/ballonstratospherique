/* 
 * File:   MPU6050.cpp
 * Author: philippe SIMIER (Lycée Touchard Le Mans)
 * 
 * Created on 31 juillet 2024, 17:06
 */

#include "MPU6050.h"

MPU6050::MPU6050() :
callback_FFD(nullptr),
callback_ZMD(nullptr),
callback_MD(nullptr),
deviceI2C(nullptr),
gpio_int(26) {

}

void MPU6050::begin(int8_t address) {

    deviceI2C = new i2c(address);
    // Configuration du registre d'alimentation
    deviceI2C->WriteReg8(PWR_MGMT_1, 0x00);

    //keep maximum sample rate 1kHz
    deviceI2C->WriteReg8(SAMPLRT_DIV, 0x00);
    disableInterupt();

    // Lecture du registre d'identification
    unsigned char id = deviceI2C->ReadReg8(WHO_AM_I);


    if (id != 0x68) {
        throw std::runtime_error("Exception identification MPU5060");
    }

    setDLPFMode(MPU6050::DLPF_5); // Filtrage passe bas 5Hz
    // lecture de la sensibilité dans le  registre de configuration
    sensibilityAcc = deviceI2C->ReadReg8(ACCEL_CONFIG) & SENSIBILITY_MASK;
    sensibilityGyr = deviceI2C->ReadReg8(GYRO_CONFIG) & SENSIBILITY_MASK;

    if (wiringPiSetupGpio() == -1) {
        throw std::runtime_error("Exception initialisation de WiringPi MPU5060");
    }

    pinMode(gpio_int, INPUT);
    pullUpDnControl(gpio_int, PUD_OFF); // La sortie int du MPU est push pull

    // appelle de la fonction ISR_Function sur interruption au front montant de gpio_int
    if (wiringPiISR(gpio_int, INT_EDGE_RISING, &interruptHandler) < 0) {
        throw std::runtime_error("Exception in begin gpio_int");
    }

}

MPU6050::~MPU6050() {
    if (deviceI2C != nullptr)
        delete deviceI2C;
}

/**
 * 
 * @return float la température en degré celcius
 */
float MPU6050::getTemperature() {

    data temp;
    temp.uCData[1] = deviceI2C->ReadReg8(TEMP_OUT_H);
    temp.uCData[0] = deviceI2C->ReadReg8(TEMP_OUT_L);

    return (float) temp.sData / 340.0 + 36.53;
}

/**
 * 
 * @return float Accélération axe Z en g
 */
float MPU6050::getAccelZ() {


    data dataAccel;
    dataAccel.uCData[1] = deviceI2C->ReadReg8(ACCEL_ZOUT_H);
    dataAccel.uCData[0] = deviceI2C->ReadReg8(ACCEL_ZOUT_L);
    float val{0.0};

    switch (sensibilityAcc) {
        case FS_2G:
            val = dataAccel.sData / 16384.0;
            break;
        case FS_4G:
            val = dataAccel.sData / 8192.0;
            break;
        case FS_8G:
            val = dataAccel.sData / 4096.0;
            break;
        case FS_16G:
            val = dataAccel.sData / 2048.0;
            break;
    }
    return val;
}

/**
 * 
 * @return float Accélération axe Y en g
 */

float MPU6050::getAccelY() {

    data dataAccel;
    dataAccel.uCData[1] = deviceI2C->ReadReg8(ACCEL_YOUT_H);
    dataAccel.uCData[0] = deviceI2C->ReadReg8(ACCEL_YOUT_L);

    float val{0.0};

    switch (sensibilityAcc) {
        case FS_2G:
            val = dataAccel.sData / 16384.0;
            break;
        case FS_4G:
            val = dataAccel.sData / 8192.0;
            break;
        case FS_8G:
            val = dataAccel.sData / 4096.0;
            break;
        case FS_16G:
            val = dataAccel.sData / 2048.0;
            break;
    }
    return val;


}

/**
 * 
 * @return float Accélération axe X en g
 */
float MPU6050::getAccelX() {

    data dataAccel;
    dataAccel.uCData[1] = deviceI2C->ReadReg8(ACCEL_XOUT_H);
    dataAccel.uCData[0] = deviceI2C->ReadReg8(ACCEL_XOUT_L);

    float val{0.0};

    switch (sensibilityAcc) {
        case FS_2G:
            val = dataAccel.sData / 16384.0;
            break;
        case FS_4G:
            val = dataAccel.sData / 8192.0;
            break;
        case FS_8G:
            val = dataAccel.sData / 4096.0;
            break;
        case FS_16G:
            val = dataAccel.sData / 2048.0;
            break;
    }
    return val;
}

void MPU6050::getMotion3(int16_t &ax, int16_t &ay, int16_t &az) {

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

/**
 * @brief MPU6050::getAccelM
 *        Calcul le module de l'accélération
 * @return accélération totale ressentie par le capteur
 */
float MPU6050::getAccelM() {

    auto accX = getAccelX();
    auto accY = getAccelY();
    auto accZ = getAccelZ();
    return sqrt(accZ * accZ + accX * accX + accY * accY);
}

/**
 * 
 * @return la vitesse de rotation autour de X en °/s
 */
float MPU6050::getRotationX() {

    data dataGyro;
    dataGyro.uCData[1] = deviceI2C->ReadReg8(GYRO_XOUT_H);
    dataGyro.uCData[0] = deviceI2C->ReadReg8(GYRO_XOUT_L);

    float val{0.0};

    switch (sensibilityGyr) {
        case FS_250DPS:
            val = (float) (dataGyro.sData * 250) / 32768;
            break;
        case FS_500DPS:
            val = (float) (dataGyro.sData * 500) / 32768;
            break;
        case FS_1000DPS:
            val = (float) (dataGyro.sData * 1000) / 32768;
            break;
        case FS_2000DPS:
            val = (float) (dataGyro.sData * 2000) / 32768;
            break;
    }
    return val;


}

float MPU6050::getRotationY() {

    data dataGyro;
    dataGyro.uCData[1] = deviceI2C->ReadReg8(GYRO_YOUT_H);
    dataGyro.uCData[0] = deviceI2C->ReadReg8(GYRO_YOUT_L);

    float val{0.0};
    switch (sensibilityGyr) {
        case FS_250DPS:
            val = (float) (dataGyro.sData * 250) / 32768;
            break;
        case FS_500DPS:
            val = (float) (dataGyro.sData * 500) / 32768;
            break;
        case FS_1000DPS:
            val = (float) (dataGyro.sData * 1000) / 32768;
            break;
        case FS_2000DPS:
            val = (float) (dataGyro.sData * 2000) / 32768;
            break;
    }
    return val;

}

float MPU6050::getRotationZ() {

    data dataGyro;
    dataGyro.uCData[1] = deviceI2C->ReadReg8(GYRO_ZOUT_H);
    dataGyro.uCData[0] = deviceI2C->ReadReg8(GYRO_ZOUT_L);

    float val{0.0};
    switch (sensibilityGyr) {
        case FS_250DPS:
            val = (float) (dataGyro.sData * 250) / 32768;
            break;
        case FS_500DPS:
            val = (float) (dataGyro.sData * 500) / 32768;
            break;
        case FS_1000DPS:
            val = (float) (dataGyro.sData * 1000) / 32768;
            break;
        case FS_2000DPS:
            val = (float) (dataGyro.sData * 2000) / 32768;
            break;
    }
    return val;

}

void MPU6050::getGyro3(int16_t &gx, int16_t &gy, int16_t &gz) {

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

/**
 * @brief methode pour configurer la sensibilité de l'accélérometre.
 * @param range FS_2G ou FS_4G ou FS_8G ou FS_16G
 */
void MPU6050::setAccSensibility(Sensibility range) {

    char val0 = deviceI2C->ReadReg8(ACCEL_CONFIG) & ~SENSIBILITY_MASK;
    deviceI2C->WriteReg8(ACCEL_CONFIG, val0 | range);
    sensibilityAcc = deviceI2C->ReadReg8(ACCEL_CONFIG) & SENSIBILITY_MASK;
}

/**
 * @brief methode pour configurer la sensibilité du gyroscope
 * @param range FS_250DPS FS_500DPS FS_1000DPS FS_2000DPS
 */
void MPU6050::setGyroSensibility(MPU6050::Sensibility range) {

    char val0 = deviceI2C->ReadReg8(GYRO_CONFIG) & ~SENSIBILITY_MASK;
    deviceI2C->WriteReg8(GYRO_CONFIG, val0 | range);
    sensibilityGyr = deviceI2C->ReadReg8(GYRO_CONFIG) & SENSIBILITY_MASK;

}

/**
 * @brief MPU6050::setDLPFMode méthode pour configurer le filtre passe bas
 * @param dlpf de DLPF_0 à DLPF_6
 */
void MPU6050::setDLPFMode(Dlpf dlpf) {

    char val0 = deviceI2C->ReadReg8(CONFIG) & 0xF8;
    deviceI2C->WriteReg8(CONFIG, val0 | dlpf);

}

void MPU6050::setAccelOffset(int16_t offsetX, int16_t offsetY, int16_t offsetZ) {
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

/**
 * @brief MPU6050::calibrate
 * @abstract calcule la valeur des offsets
 *           Assumer que l'axe Z pointe vers le haut (+1g)
 *           Lors de la calibration, les valeurs de sortie du capteur sont comparées à des valeurs de référence connues.
 *           Les différences observées sont utilisées pour calculer les valeurs d'offset
 *           qui sont ensuite stockées dans les registres correspondants.
 *           Cette étape est essentielle pour assurer que le capteur fournit des mesures précises et cohérentes.
 */
void MPU6050::calibrateA() {

    int16_t ax, ay, az, ox, oy, oz;
    int ready, i = 0;

    setAccSensibility(MPU6050::FS_2G);
    setAccelOffset(0, 0, 0);

    meansensorsA(200, ax, ay, az);
    cout << "i 0 => " << ax << " , " << ay << " , " << az << endl;
    // Calcul des offsets
    ox = -ax / 8;
    oy = -ay / 8;
    oz = (16384 - az) / 8;


    do {
        i++;
        if (i > 35) {
            throw std::runtime_error("Exception calibrate acc MPU5060");
        }
        ready = 0;
        setAccelOffset(ox, oy, oz);

        meansensorsA(100, ax, ay, az);
        cout << "i  " << i << " => " << ax << " , " << ay << " , " << az << endl;

        if (ax > 8 || ax < -8)
            ox = ox - ax / 8;
        else ready++;
        if (ay > 8 || ay < -8)
            oy = oy - ay / 8;
        else ready++;
        if (((az - 16384) > 8) || ((az - 16384) < -8))
            oz = oz - (az - 16384) / 8;
        else ready++;
    } while (ready < 3);

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

void MPU6050::calibrateG() {
    int16_t gx, gy, gz, ox, oy, oz;
    int ready[3] = {0, 0, 0};
    int i = 0;

    setGyroSensibility(MPU6050::FS_250DPS);
    setGyroOffset(0, 0, 0);

    meansensorsG(100, gx, gy, gz);
    cout << "i 0 => " << gx << " , " << gy << " , " << gz << endl;
    // Calcul des offsets
    ox = -gx / 4;
    oy = -gy / 4;
    oz = -gz / 4;


    do {
        i++;
        if (i > 35) {
            throw std::runtime_error("Exception calibrate gyro MPU5060");
        }

        setGyroOffset(ox, oy, oz);

        meansensorsG(100, gx, gy, gz);
        cout << "i  " << i << " => " << gx << " , " << gy << " , " << gz << endl;

        if ((gx != 0) && !ready[0])
            ox = ox - gx / 8;
        else ready[0] = 1;
        if ((gy != 0) && !ready[1])
            oy = oy - gy / 8;
        else ready[1] = 1;
        if ((gz != 0) && !ready[2])
            oz = oz - gz / 8;
        else ready[2] = 1;
    } while (!ready[0] && !ready[1] && !ready[2]);
}

void MPU6050::enableMotion(uint8_t thresold, uint8_t duration) {
    deviceI2C->ReadReg8(INT_STATUS);
    deviceI2C->WriteReg8(INT_PIN_CONFIG, 0x20); //bit 5 à 1 for setting active-high interupt cleared by reading INT_STATUS.
    uint8_t value = deviceI2C->ReadReg8(INT_ENABLE);
    deviceI2C->WriteReg8(INT_ENABLE, value | 0x40); // Bit 6 à 1 for Motion Detection
    deviceI2C->WriteReg8(MOT_THR, thresold);
    deviceI2C->WriteReg8(MOT_DUR, duration);

}

void MPU6050::enableFreeFall(uint8_t thresold, uint8_t duration) {
    deviceI2C->ReadReg8(INT_STATUS);
    deviceI2C->WriteReg8(INT_PIN_CONFIG, 0x20); //bit 5 à 1 for setting active-high interupt cleared by reading INT_STATUS.
    uint8_t value = deviceI2C->ReadReg8(INT_ENABLE);
    deviceI2C->WriteReg8(INT_ENABLE, value | 0x80); // Bit 7 à 1 for Free Fall
    deviceI2C->WriteReg8(FF_THR, thresold);
    deviceI2C->WriteReg8(FF_DUR, duration);
}

void MPU6050::enableZeroMotion(uint8_t thresold, uint8_t duration) {
    deviceI2C->ReadReg8(INT_STATUS);
    deviceI2C->WriteReg8(INT_PIN_CONFIG, 0x20); //bit 5 à 1 for setting active-high interupt cleared by reading INT_STATUS.
    uint8_t value = deviceI2C->ReadReg8(INT_ENABLE);
    deviceI2C->WriteReg8(INT_ENABLE, value | 0x20); // Bit 5 à 1 for zero motion detection
    deviceI2C->WriteReg8(ZRMOT_THR, thresold);
    deviceI2C->WriteReg8(ZRMOT_DUR, duration);
}

void MPU6050::disableInterupt() {
    deviceI2C->WriteReg8(INT_ENABLE, 0x00); // Bit 6 à 0
}

uint8_t MPU6050::getStatusInt() {
    return deviceI2C->ReadReg8(INT_STATUS);
}

void MPU6050::onFreeFall(void (*userFunct)(void)) {
    callback_FFD = userFunct;
}

void MPU6050::onMotion(void (*userFunct)(void)) {
    callback_MD = userFunct;
}

void MPU6050::onZeroMotion(void (*userFunct)(void)) {
    callback_ZMD = userFunct;
}

void MPU6050::interruptHandler() {

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Attendre 100 ms;
    auto value = mpu.getStatusInt();

    if ((value & 0x80) == 0x80) {
        if (mpu.callback_FFD != nullptr)
            mpu.callback_FFD();

    }

    if ((value & 0x40) == 0x40) {
        if (mpu.callback_MD != nullptr)
            mpu.callback_MD();
    }

    if ((value & 0x20) == 0x20) {
        if (mpu.callback_ZMD != nullptr)
            mpu.callback_ZMD();
    }
}

MPU6050::Sensibility MPU6050::sensibilityFromString(const std::string& str) {
    
    if (str == "FS_2G")  return FS_2G;
    if (str == "FS_4G")  return FS_4G;
    if (str == "FS_8G")  return FS_8G;
    if (str == "FS_16G") return FS_16G;
    if (str == "FS_250DPS") return FS_250DPS;
    if (str == "FS_500DPS") return FS_500DPS;
    if (str == "FS_1000DPS") return FS_1000DPS;
    if (str == "FS_2000DPS") return FS_2000DPS;
    throw std::invalid_argument("Invalid MPU6050 sensibility String");
}

MPU6050::Dlpf MPU6050::dlpfFromString(const std::string& str) {
    
    if (str == "DLPF_260")  return DLPF_260;
    if (str == "DLPF_184")  return DLPF_184;
    if (str == "DLPF_94")  return DLPF_94;
    if (str == "DLPF_44")  return DLPF_44;    
    if (str == "DLPF_21")  return DLPF_21;
    if (str == "DLPF_10")  return DLPF_10;
    if (str == "DLPF_5")  return DLPF_5;
    throw std::invalid_argument("Invalid MPU6050 dlpf String");
}

MPU6050 mpu;