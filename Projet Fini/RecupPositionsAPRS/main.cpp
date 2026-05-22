/**
 * @file main.cpp
 * @brief Point d'entrée de l'application APRS.fi
 * @details Initialise l'application Qt et affiche la fenêtre principale
 * @version 4.0
 * @date 22/05/2026
 * @author Guerriau Lucien
 */

#include <QApplication>
#include "aprsfi.h"

/**
 * @brief Point d'entrée principal
 * @param argc Nombre d'arguments de la ligne de commande
 * @param argv Tableau des arguments de la ligne de commande
 * @return Code de retour de l'application Qt
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    aprsfi w;
    w.show();
    return a.exec();
}
