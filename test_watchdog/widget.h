#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileSystemWatcher>
#include "bdd.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onDossierModifie(const QString &path);

private:
    Ui::Widget *ui;
    QFileSystemWatcher *watcher;
    Bdd *maSql; // On l'appelle GestionBDD pour ne pas confondre avec le Widget
    QStringList imagesTraitees;
};

#endif // WIDGET_H
