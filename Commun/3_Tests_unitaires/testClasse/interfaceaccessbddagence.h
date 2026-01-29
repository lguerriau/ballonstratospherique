#ifndef INTERFACEACCESSBDDAGENCE_H
#define INTERFACEACCESSBDDAGENCE_H

#include <QObject>
#include <QSqlDatabase>
#include "avion.h"
#include <QJsonArray>

class InterfaceAccessBddAgence : public QObject
{
    Q_OBJECT
public:
    explicit InterfaceAccessBddAgence(QObject *parent = nullptr);

    bool ajouterReservation(int ref, int idClient, int nroSiege);
    bool supprimerReservation(int idResa);
    QList <avion *>obtenirListeDesVols();
    int ajouterClient(QString nom, QString prenom,QString email);
    QJsonArray obtenirListeReservations();
    int ajouterVol(QString vol);

private:
    QSqlDatabase bdd;
    QString login;
    QString motDePasse;
    QString adresse;
    QString nomBase;

    void chargerParametresBdd();



};

#endif // INTERFACEACCESSBDDAGENCE_H
