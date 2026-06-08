#include "DataLogger.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <iostream>

DataLogger::DataLogger(const std::string& filepath) : filepath(filepath) {}

std::string DataLogger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

void DataLogger::log(float tempC, float hum, float press, const std::string& trame) {
    std::ofstream f(filepath, std::ios::app);
    if (!f.is_open()) {
        std::cerr << "[DataLogger] Impossible d'ouvrir : " << filepath << std::endl;
        return;
    }
    f << std::fixed << std::setprecision(2)
      << "{\"ts\":\"" << getTimestamp() << "\""
      << ",\"t\":"   << tempC
      << ",\"h\":"   << hum
      << ",\"p\":"   << press
      << ",\"aprs\":\"" << trame << "\"}"
      << std::endl;
}