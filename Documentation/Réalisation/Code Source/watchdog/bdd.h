#ifndef BDD_H
#define BDD_H

#include <QWidget>
#include <QtSql>
#include <QSqlDatabase>
#include <QSettings>
#include <QString>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class Bdd; }
QT_END_NAMESPACE

class Bdd : public QWidget
{
    Q_OBJECT

public:
    Bdd(QWidget *parent = nullptr);


    bool connecter();
    void deconnecter();
    bool enregistrerPhoto(QString cheminLocal, bool pourWeb);

private:
    QSqlDatabase db;

    QString host;
    QString user;
    QString password;
    QString dbName;

    void chargerConfiguration();
};

#endif // BDD_H
