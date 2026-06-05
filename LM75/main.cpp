/* * File:   main.cpp
 * Author: philippe SIMIER
 *
 * Created on 25 juillet 2023, 15:54
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "LM75.h"
#include <unistd.h>
#include <chrono>    // Pour l'horodatage
#include <fstream>   // Pour l'écriture de fichiers
#include <sstream>   // Pour le formatage de la date

using namespace std;

/**
 * @brief Génère un horodatage textuel précis basé sur l'heure système.
 * @return string Chaîne de caractères au format "AAAA-MM-JJ HH:MM:SS".
 */
string getTimestamp() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

/**
 * @brief Point d'entrée de l'application de suivi thermique.
 * @details Initialise le capteur LM75, puis effectue une acquisition continue 
 * toutes les secondes pour un affichage sur la console et une sauvegarde 
 * historique dans un fichier local au format JSON.
 * @param argc Nombre d'arguments en ligne de commande.
 * @param argv Tableau des arguments en ligne de commande.
 * @return int Code de retour de l'application (0 en cas de fonctionnement normal).
 */
int main(int argc, char** argv) {

    try {
        LM75 capteur;
        string nomFichier = "mesures_lm75.json";

        while (1) {
            // Lecture unique de la température pour l'affichage et le log
            float t = capteur.getTemperature();
            string date = getTimestamp();

            // Affichage console (inchangé par rapport à ta version originale)
            cout << setprecision(3) << t << "°C\r" <<endl;

            // Enregistrement JSON en mode ajout (append)
            ofstream fichier(nomFichier, ios::app);
            if (fichier.is_open()) {
                fichier << "{"
                        << "\"date\":\"" << date << "\", "
                        << "\"temperature\":" << fixed << setprecision(3) << t
                        << "}" << endl;
                fichier.close();
            }

            sleep(1);
        }
    } catch (const runtime_error &e) {
        cout << "Exception caught: " << e.what() << endl;
    }

    return 0;
}