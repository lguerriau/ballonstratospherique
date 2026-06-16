/**
   @file watchdog.h
   @brief Déclaration de la classe Watchdog
   @version 1.0
   @author Harold KALO
   @date 26/05/2026
*/

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <QWidget>
#include <QStringList>
#include <QTimer>
#include "bdd.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Watchdog : public QWidget {
    Q_OBJECT

public:
    explicit Watchdog(QWidget *parent = nullptr);
    virtual ~Watchdog() override;

    void chargerConfiguration();
private slots:
    /** Slot déclenché par le timer pour inspecter le dossier réseau. */
    void onDossierModifie(const QString &path);

private:
    /** Pointeur vers l'interface. */
    Ui::Widget *ui;

    /** Horloge cyclique cadençant l'interrogation du répertoire partagé. */
    QTimer timerApp;

    /** Composant d'interface locale pour les transactions avec MariaDB. */
    Bdd baseDeDonneesImages;
    QString repertoireDestination;
    QString repertoireSource;

    /** @brief Liste mémoire stockant le nom des fichiers images déjà traités. */
    QStringList imagesTraitees;
};

#endif // WATCHDOG_H
