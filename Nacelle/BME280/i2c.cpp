/*!
    \file     i2c.cpp
    \author   Philippe SIMIER (Touchard Wahington le Mans)
    \license  BSD (see license.txt)
    \brief    Classe pour le bus I2C
    \detail  I2C (pronounce: I squared C) is a protocol developed by Philips. It is a
             slow two-wire protocol (variable speed, up to 400 kHz), with a high speed
             extension (3.4 MHz).  It provides an inexpensive bus for connecting many
             types of devices with in frequent or low bandwidth communications needs.
             I2C is widely used with embedded systems.
    \version v1.0 - First release
 */

#include "i2c.h"

// Le constructeur

i2c::i2c(int adresseI2C, int idBusI2C) {
    char filename[20];
    union i2c_smbus_data data;

    snprintf(filename, 19, "/dev/i2c-%d", idBusI2C);
    if ((fd = open(filename, O_RDWR)) < 0) {
        cout << "Erreur d'ouverture du bus I2C" << endl;
        exit(1);
    }
    //  Change slave address. The address is passed in the
    //  7 lower bits of the  argument
    if (ioctl(fd, I2C_SLAVE, adresseI2C) < 0) {
        cout << "Impossible de sélectionner l'adresse I2C" << endl;
        exit(1);
    }
    // test de la présence du composant sur le bus
    if (i2c_smbus_access(I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data)) {
        error = true;
    } else {
        error = false;
    }
}

bool i2c::getError() {
    return error;
}

int i2c::i2c_smbus_access(char rw, uint8_t command, int size, union i2c_smbus_data *data) {
    struct i2c_smbus_ioctl_data args;

    args.read_write = rw;
    args.command = command;
    args.size = size;
    args.data = data;
    return ioctl(fd, I2C_SMBUS, &args);
}

unsigned char i2c::Read() {
    union i2c_smbus_data data;
    unsigned char valeur = 0x00;
    if (i2c_smbus_access(I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data))
        error = true;
    else
        valeur = data.byte & 0xFF;
    return valeur;
}

unsigned char i2c::ReadReg8(int reg) {
    union i2c_smbus_data data;
    unsigned char valeur = 0x00;
    if (i2c_smbus_access(I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data))
        error = true;
    else
        valeur = data.byte & 0xFF;
    return valeur;
}

unsigned short i2c::ReadReg16(int reg) {
    union i2c_smbus_data data;
    unsigned short valeur = 0x0000;
    if (i2c_smbus_access(I2C_SMBUS_READ, reg, I2C_SMBUS_WORD_DATA, &data))
        error = true;
    else
        valeur = data.word & 0xFFFF;
    return valeur;
}

unsigned char i2c::Write(int data) {
    return i2c_smbus_access(I2C_SMBUS_WRITE, data, I2C_SMBUS_BYTE, NULL);

}

unsigned char i2c::WriteReg8(int reg, int value) {
    union i2c_smbus_data data;

    data.byte = value;
    return i2c_smbus_access(I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data);
}

unsigned short i2c::WriteReg16(int reg, int value) {
    union i2c_smbus_data data;

    data.word = value;
    return i2c_smbus_access(I2C_SMBUS_WRITE, reg, I2C_SMBUS_WORD_DATA, &data);
}

int i2c::WriteBlockData(int reg, int length, int *values) {
    union i2c_smbus_data data;
    int i;
    if (length > 32)
        length = 32;
    for (i = 1; i <= length; i++)
        data.block[i] = values[i - 1];
    data.block[0] = length;
    return i2c_smbus_access(I2C_SMBUS_WRITE, reg, I2C_SMBUS_I2C_BLOCK_BROKEN, &data);
}

int i2c::ReadBlockData(int reg, int length, int *values) {
    union i2c_smbus_data data;
    int i;

    if (length > 32)
        length = 32;
    data.block[0] = length;
    if (i2c_smbus_access(I2C_SMBUS_READ, reg, length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN :
            I2C_SMBUS_I2C_BLOCK_DATA, &data)) {
        error = true;
        return -1;
    } else {
        for (i = 1; i <= data.block[0]; i++)
            values[i - 1] = data.block[i];
        return data.block[0];
    }
}
// retourne le nombre d'octets lu

int i2c::delay_ms(unsigned long num_ms) {
    struct timespec ts;

    ts.tv_sec = num_ms / 1000;
    ts.tv_nsec = (num_ms % 1000) * 1000000;

    return nanosleep(&ts, NULL);
}
