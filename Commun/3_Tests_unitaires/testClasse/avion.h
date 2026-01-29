#include <QString>
#include <QDataStream>
#include <QList>
#ifndef AVION_H
#define AVION_H
struct vol{
    int reference;
    QString denomination;
    friend QDataStream& operator>>(QDataStream& in, vol& v) {
            in >> v.reference >> v.denomination;
            return in;
        }
    friend QDataStream& operator<<(QDataStream& out, const vol& v) {
           out << v.reference << v.denomination;
           return out;
       }
};
struct avion{
    vol infosVol;
    QList <int>siegesOccupees;
};

#endif // AVION_H
