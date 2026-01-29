#include <QCoreApplication>
#include "interfaceaccessbddagence.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);
    InterfaceAccessBddAgence bdd;
    cout<<bdd.obtenirListeDesVols();
}
