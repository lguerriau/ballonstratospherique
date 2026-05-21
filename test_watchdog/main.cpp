#include "watchdog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Watchdog w;
    w.show();
    return a.exec();
}
