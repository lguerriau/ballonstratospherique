/**
 * @file AprsBuilder.h
 * @brief Gestionnaire de formatage des trames au standard APRS Weather.
 */

#ifndef APRS_BUILDER_H
#define APRS_BUILDER_H

#include <string>

/**
 * @class AprsBuilder
 * @brief Permet d'encapsuler et de formater les données environnementales sous forme de chaînes conformes au protocole APRS.
 */
class AprsBuilder {
private:
    std::string indicatif; /*!< Indicatif radio (Callsign) de la station (ex: F4KMN-9) */
    
    /**
     * @brief Génère l'horodatage UTC requis par le protocole APRS.
     * @return Chaîne de caractères au format MMDDHHMM (Mois, Jour, Heure, Minute).
     */
    std::string getAPRSTimestamp() const;
    
    /**
     * @brief Vérifie les limites physiques des données avant l'encapsulation.
     * @param tempF Température en degrés Fahrenheit.
     * @param hum Taux d'humidité en %.
     * @param press Pression atmosphérique absolue en hPa.
     * @return true si toutes les mesures sont réalistes, false sinon.
     */
    bool validerDonneesCapteurs(float tempF, float hum, float press) const;

public:
    /**
     * @brief Constructeur de la classe AprsBuilder.
     * @param callsign Indicatif de la station météo avec son SSID.
     */
    AprsBuilder(const std::string& callsign);
    
    /**
     * @brief Assemble les différentes mesures pour produire la trame textuelle APRS finale.
     * @param tempF Température convertie en Fahrenheit.
     * @param hum Humidité relative (%).
     * @param press Pression atmosphérique (hPa).
     * @return string Trame APRS complète prête pour l'émission ou message d'erreur.
     */
    std::string buildTrame(float tempF, float hum, float press) const;
};

#endif