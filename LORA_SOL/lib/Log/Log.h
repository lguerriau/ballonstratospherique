#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

class Log {
public:
    bool enregistrerErreur(const char* msg);
};

#endif