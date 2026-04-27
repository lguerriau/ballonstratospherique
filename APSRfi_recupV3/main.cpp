#include "aprsfi.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    aprsfi w;
    w.show();
    return a.exec();
}
