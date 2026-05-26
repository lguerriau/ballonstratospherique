/**
 * @file widget.h
 * @brief Déclaration de la classe Widget (Interface Graphique)
 * @version 1.0
 * @author nbrands
 * @date 26/05/2026
 * @details Fenêtre principale de l'application Qt. Supervise l'affichage 
 * des données télémétriques, des alertes de vol et des indicateurs radio.
 */

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QElapsedTimer>
#include <QDateTime>
#include "communicationlora.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur de l'interface graphique
     * @param parent Pointeur vers le widget parent (nullptr pour la fenêtre principale)
     */
    Widget(QWidget *parent = nullptr);
    
    /**
     * @brief Destructeur de l'interface
     */
    ~Widget();

private slots:
    /**
     * @brief Slot déclenché lors de l'appui sur le bouton "Envoyer"
     * @details Déclenche la demande de transmission vers la Gateway et lance le chronomètre de latence.
     */
    void on_btn_envoyer_clicked();

    /**
     * @brief Slot déclenché à la réception d'un nouveau message depuis la Gateway
     * @param message Trame texte extraite du port série
     * @details Aiguille l'affichage (Tableau, Alertes de vol, Ignorance des trames météo).
     */
    void lireDonneesSerie(const QString &message);

private:
    /**
     * @brief Met à jour le bandeau de statut de vol et son design (QSS)
     * @param status Identifiant du statut ("BURST", "LANDING", "en vol")
     */
    void updateFlightStatus(const QString &status); 

    /** Pointeur vers l'objet généré par Qt Designer (Interface UI) */
    Ui::Widget *ui;
    
    /** Pointeur vers le gestionnaire métier de la communication série */
    CommunicationLora *lora;

    /** Chronomètre haute précision pour mesurer le délai (Ping) d'un aller-retour radio */
    QElapsedTimer timerLatence;
};

#endif // WIDGET_H
