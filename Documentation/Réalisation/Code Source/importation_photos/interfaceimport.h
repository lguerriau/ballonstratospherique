#ifndef INTERFACEIMPORT_H
#define INTERFACEIMPORT_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget> // Pour afficher proprement la liste des fichiers
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
    void connecterEtLister();
    void importerPhotos();

private:
    Ui::InterfaceImport *ui;
    QLineEdit *champIP;
    QLineEdit *champMdp;
    QLineEdit *champDossierLocal;
    QPushButton *boutonConnexion;
    QPushButton *boutonImporter;
    QListWidget *listeFichiersVisuels;
    QTextEdit *zoneLogs;
};
#endif // INTERFACEIMPORT_H
