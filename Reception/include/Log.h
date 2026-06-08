#ifndef LOG_H
#define LOG_H
#include <Arduino.h>

class Log {
public:
    void enregistrerErreur(String message);
};
#endif