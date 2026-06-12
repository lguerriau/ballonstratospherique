/**
   @file bdd.h
   @brief Déclaration de la classe Bdd
   @version 1.0
   @author Harold KALO
   @date 26/05/2026
*/

#ifndef BDD_H
#define BDD_H

#include <QWidget>
#include <QtSql>
#include <QSqlDatabase>
#include <QSettings>
#include <QString>>

QT_BEGIN_NAMESPACE
namespace Ui { class Bdd; }
QT_END_NAMESPACE

class Bdd : public QWidget
{
    Q_OBJECT

public:
    explicit Bdd(QWidget *parent = nullptr);
    virtual ~Bdd() override;

    bool connecter();
    void deconnecter();
    bool enregistrerPhoto(const QString &cheminFichier, bool pourWeb);
private:
    /** La bdd utilisée*/
    QSqlDatabase db;

    /** l'adresse ip de la bdd*/
    QString host;

    /** le nom d'utilisateur de la bdd*/
    QString user;

    /** le mot de passe de la bdd*/
    QString password;

    /** le nom de la bdd*/
    QString dbName;

    void chargerConfiguration();
};

#endif // BDD_H
