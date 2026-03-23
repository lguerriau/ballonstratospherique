#include <QApplication> // <-- On utilise QApplication au lieu de QCoreApplication
#include "aprsfi.h"

int main(int argc, char *argv[]) {
    // QApplication gère les fenêtres, les clics de souris, etc.
    QApplication a(argc, argv);

    // On crée et on affiche ton widget
    aprsfi w;
    w.show();

    // On lance la machine (API, BDD, WebSocket)
    w.startBackend();

    return a.exec();
}
