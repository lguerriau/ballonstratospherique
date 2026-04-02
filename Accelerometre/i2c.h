#ifndef I2C_H_INCLUDED
#define I2C_H_INCLUDED


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <asm/ioctl.h>
#include <iostream>



// I2C definitions

#define I2C_SLAVE       0x0703  /* Use this slave address */
#define I2C_ACK_TEST    0x0710  /* Voir si un esclave est à une adresse spécifique */
#define I2C_SMBUS       0x0720  /* SMBus transfer */


#define I2C_SMBUS_READ  1
#define I2C_SMBUS_WRITE 0

// SMBus transaction types

#define I2C_SMBUS_QUICK             0  // This sends a single bit to the device, at the place of the Rd/Wr bit.
#define I2C_SMBUS_BYTE              1  // Handles the SMBus read_byte and write_byte commands
#define I2C_SMBUS_BYTE_DATA         2  // Handles the SMBus read_byte_data and write_byte_data commands
#define I2C_SMBUS_WORD_DATA         3  // Handles the SMBus read_word_data and write_word_data commands
#define I2C_SMBUS_PROC_CALL         4  // This command selects a device register (through the Comm byte), sends
                                       // 16 bits of data to it, and reads 16 bits of data in return.
#define I2C_SMBUS_BLOCK_DATA        5  // Handles the SMBus read_block_data and write_block_data commands
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6  // 
#define I2C_SMBUS_BLOCK_PROC_CALL   7  // This command selects a device register (through the Comm byte), sends
                                       // 1 to 31 bytes of data to it, and reads 1 to 31 bytes of data in return.
#define I2C_SMBUS_I2C_BLOCK_DATA    8

// SMBus messages

#define I2C_SMBUS_BLOCK_MAX     32      /* taille maxi d'un bloc de données */
#define I2C_SMBUS_I2C_BLOCK_MAX 32      /* Not specified but we use same structure */

// Structures utilisées par les appels ioctl()

using namespace std;

// La donnée peut être soit un Octet, soit un Mot ou un tableau d'octet
union i2c_smbus_data
{
  uint8_t  byte ;
  uint16_t word ;
  uint8_t  block [I2C_SMBUS_BLOCK_MAX + 2] ;    // 2 car block [0] est utilisé pour la longeur + 1 pour PEC (Controle CRC)
};

struct i2c_smbus_ioctl_data
{
  char read_write ;
  uint8_t command ;
  int size ;
  union i2c_smbus_data *data ;
};



class i2c
{

    public:

        // le constructeur
        i2c(int adresseI2C, int idBusI2C=1);
        //idBusI2C = 0 pour les raspberry version 1
        //idBusI2C = 1 pour les raspberry version 2 et 3

        bool getError();

        unsigned char Read ();
        unsigned char ReadReg8 (int reg);
        unsigned short ReadReg16 (int reg);
        int ReadBlockData (int reg, int length, int *values);

        unsigned char Write (int data);
        unsigned char WriteReg8 (int reg, int value);
        unsigned short WriteReg16 (int reg, int value);
        int WriteBlockData (int reg, int length, int *data);
        int delay_ms(unsigned long num_ms);

    private:

        int fd;
        bool error;
        inline int i2c_smbus_access (char rw, uint8_t command, int size, union i2c_smbus_data *data);

};

#endif // I2C_H_INCLUDED