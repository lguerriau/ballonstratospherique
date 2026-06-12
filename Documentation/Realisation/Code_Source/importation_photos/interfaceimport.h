/**
   @file interfaceimport.h
   @brief Déclaration de la classe Interfaceimport
   @version 1.0
   @author Harold KALO
   @date 04/06/2026
*/

#ifndef INTERFACEIMPORT_H
#define INTERFACEIMPORT_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class InterfaceImport; }
QT_END_NAMESPACE

class InterfaceImport : public QWidget
{
    Q_OBJECT

public:
    InterfaceImport(QWidget *parent = nullptr);
    ~InterfaceImport();

private slots:
    /** Slot déclenché par le process pour lister le dossier photos. */
    void connecterEtLister();

    /** Slot déclenché par le process pour importer les fichiers photos vers le dossier local de destination. */
    void importerPhotos();

private:
    /** Pointeur vers l'interface. */
    Ui::InterfaceImport *ui;

    /** Champ pour inscrire l'adresse ip de la raspberry. */
    QLineEdit *champIP;

    /** Champ pour inscrire le mot de passe de la raspberry. */
    QLineEdit *champMdp;

    /** Champ pour inscrire le dossier local de destination où les photos seront importées. */
    QLineEdit *champDossierLocal;

    /** Bouton pour se connecter à la raspberry. */
    QPushButton *boutonConnexion;

    /** Bouton pour se importer les photos de la raspberry vers le dossier local. */
    QPushButton *boutonImporter;

    /** Zone pour voir les fichiers photos de la raspberry . */
    QListWidget *listeFichiersVisuels;

    /** Zone pour voir les informations de connexion et d'importation de la raspberry sur l'interface. */
    QTextEdit *zoneLogs;
};
#endif // INTERFACEIMPORT_H
