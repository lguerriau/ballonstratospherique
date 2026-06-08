#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <string>

class DataLogger {
public:
    /**
     * @param filepath Chemin du fichier JSON (ex: "/home/pi/meteo.json")
     */
    explicit DataLogger(const std::string& filepath);

    /**
     * @brief Ajoute une ligne JSON avec horodatage et mesures.
     */
    void log(float tempC, float hum, float press, const std::string& trame);

private:
    std::string filepath;
    std::string getTimestamp() const;
};

#endif