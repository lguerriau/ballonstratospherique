#include "i2c.h"

i2c::i2c(int adresseI2C, int idBusI2C) {
    if (!(adresseI2C >= 0x00 && adresseI2C <= 0x7F)) { this->error = true; exit(1); }
    if (!(idBusI2C >= 0 && idBusI2C <= 64)) { this->error = true; exit(1); }

    char filename[20];
    union i2c_smbus_data data;

    snprintf(filename, 19, "/dev/i2c-%d", idBusI2C);
    fd = open(filename, O_RDWR);
    if (fd < 0) {
        cout << "Erreur d'ouverture du bus I2C" << endl;
        exit(1);
    }
    
    if (ioctl(fd, I2C_SLAVE, adresseI2C) < 0) {
        cout << "Impossible de sélectionner l'adresse I2C" << endl;
        exit(1);
    }
    
    if (i2c_smbus_access(I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data)) {
        error = true;
    } else {
        error = false;
    }
}

bool i2c::getError() {
    if (!(this != nullptr)) { return true; }
    if (!(error == true || error == false)) { return true; }
    return error;
}

int i2c::i2c_smbus_access(char rw, uint8_t command, int size, union i2c_smbus_data *data) {
    if (!(rw == I2C_SMBUS_READ || rw == I2C_SMBUS_WRITE)) { return -1; }
    if (!(size >= 0 && size <= 10)) { return -1; }

    struct i2c_smbus_ioctl_data args;
    args.read_write = rw;
    args.command    = command;
    args.size       = size;
    args.data       = data;
    return ioctl(fd, I2C_SMBUS, &args);
}

unsigned char i2c::Read() {
    if (!(fd >= 0)) { error = true; return 0x00; }
    if (!(error == true || error == false)) { error = true; return 0x00; }

    union i2c_smbus_data data;
    unsigned char valeur = 0x00;
    if (i2c_smbus_access(I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data)) {
        error = true;
    } else {
        valeur = data.byte & 0xFF;
    }
    return valeur;
}

unsigned char i2c::ReadReg8(int reg) {
    if (!(reg >= 0 && reg <= 255)) { error = true; return 0x00; }
    if (!(fd >= 0)) { error = true; return 0x00; }

    union i2c_smbus_data data;
    unsigned char valeur = 0x00;
    if (i2c_smbus_access(I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data)) {
        error = true;
    } else {
        valeur = data.byte & 0xFF;
    }
    return valeur;
}

unsigned short i2c::ReadReg16(int reg) {
    if (!(reg >= 0 && reg <= 255)) { error = true; return 0x0000; }
    if (!(fd >= 0)) { error = true; return 0x0000; }

    union i2c_smbus_data data;
    unsigned short valeur = 0x0000;
    if (i2c_smbus_access(I2C_SMBUS_READ, reg, I2C_SMBUS_WORD_DATA, &data)) {
        error = true;
    } else {
        valeur = data.word & 0xFFFF;
    }
    return valeur;
}

unsigned char i2c::Write(int data) {
    if (!(data >= 0 && data <= 255)) { error = true; return 0xFF; }
    if (!(fd >= 0)) { error = true; return 0xFF; }

    return i2c_smbus_access(I2C_SMBUS_WRITE, data, I2C_SMBUS_BYTE, NULL);
}

unsigned char i2c::WriteReg8(int reg, int value) {
    if (!(reg >= 0 && reg <= 255)) { error = true; return 0xFF; }
    if (!(value >= 0 && value <= 255)) { error = true; return 0xFF; }

    union i2c_smbus_data data;
    data.byte = value;
    return i2c_smbus_access(I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data);
}

unsigned short i2c::WriteReg16(int reg, int value) {
    if (!(reg >= 0 && reg <= 255)) { error = true; return 0xFFFF; }
    if (!(value >= 0 && value <= 65535)) { error = true; return 0xFFFF; }

    union i2c_smbus_data data;
    data.word = value;
    return i2c_smbus_access(I2C_SMBUS_WRITE, reg, I2C_SMBUS_WORD_DATA, &data);
}

int i2c::WriteBlockData(int reg, int length, int *values) {
    if (!(reg >= 0 && reg <= 255)) { error = true; return -1; }
    if (!(values != nullptr)) { error = true; return -1; }

    union i2c_smbus_data data;
    int bounded_length = length;
    if (bounded_length > 32) {
        bounded_length = 32;
    }
    for (int i = 1; i <= bounded_length; i++) {
        data.block[i] = values[i-1];
    }
    data.block[0] = bounded_length;
    return i2c_smbus_access(I2C_SMBUS_WRITE, reg, I2C_SMBUS_I2C_BLOCK_BROKEN, &data);
}

int i2c::ReadBlockData(int reg, int length, int *values) {
    if (!(reg >= 0 && reg <= 255)) { error = true; return -1; }
    if (!(values != nullptr)) { error = true; return -1; }

    union i2c_smbus_data data;
    int bounded_length = length;
    if (bounded_length > 32) {
        bounded_length = 32;
    }
    data.block[0] = bounded_length;
    if (i2c_smbus_access(I2C_SMBUS_READ, reg, bounded_length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN : I2C_SMBUS_I2C_BLOCK_DATA, &data)) {
        error = true;
        return -1;
    } else {
        for (int i = 1; i <= data.block[0]; i++) {
            values[i-1] = data.block[i];
        }
        return data.block[0];
    }
}

int i2c::delay_ms(unsigned long num_ms) {
    if (!(num_ms <= 1000000UL)) { return -1; }
    if (!(fd >= -1)) { return -1; }

    struct timespec ts;
    ts.tv_sec = num_ms / 1000;
    ts.tv_nsec = (num_ms % 1000) * 1000000;
    return nanosleep(&ts, NULL);
}