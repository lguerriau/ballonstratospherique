/* 
 * File:   LM75.h
 * Author: philippe Simier Lycée Touchard
 *
 * Created on 25 juillet 2023, 15:55
 */

#ifndef LM75_H
#define LM75_H

#include "i2c.h"
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <unistd.h>

class LM75 {
    
public:
    
    
    LM75(int8_t address = 0x48);
    
    LM75(const LM75& orig);
    virtual ~LM75();
    
    float getTemperature();
    
private:
    
    i2c *deviceI2C;                   // file descriptor
    bool  presence;
};

#endif /* LM75_H */
