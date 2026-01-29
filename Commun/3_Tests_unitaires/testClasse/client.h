#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>

class Client
{
public:
    explicit Client(QObject *parent = nullptr);
    int getReferenceVol() const;
    void setReferenceVol(int newReferenceVol);
    int getNumeroPlace() const;
    void setNumeroPlace(int newNumeroPlace);
    QString getNom() const;
    void setNom(const QString &newNom);
    QString getPrenom() const;
    void setPrenom(const QString &newPrenom);
    QString getEmail() const;
    void setEmail(const QString &newEmail);

    void setSockClient(QTcpSocket *newClient);
    QTcpSocket* getSockClient() const;

private:
    int referenceVol;
    int numeroPlace;
    QString nom;
    QString prenom;
    QString email;
    QTcpSocket *sockClient;
};

#endif // CLIENT_H
