#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QElapsedTimer>
#include <QDateTime>
#include "communicationlora.h" // On inclut ta nouvelle classe

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    // Slot déclenché par le clic sur le bouton de l'interface
    void on_btn_envoyer_clicked();

    // Slot qui sera connecté au signal "messageRecu" de ta classe CommunicationLora
    void lireDonneesSerie(const QString &message);

private:
    Ui::Widget *ui;

    // Pointeur vers ton gestionnaire de communication LoRa
    CommunicationLora *lora;

    // Chronomètre pour calculer le temps de réponse (latence)
    QElapsedTimer timerLatence;
};

#endif // WIDGET_H
