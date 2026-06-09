#include "interfaceimport.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InterfaceImport w;
    w.show();
    return a.exec();
}
