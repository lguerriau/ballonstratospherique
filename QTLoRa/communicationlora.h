/**
 * @file communicationlora.h
 * @brief Déclaration de la classe CommunicationLora
 * @version 1.0
 * @author nbrands
 * @date 26/05/2026
 * @details Classe métier gérant la communication bas niveau avec le port série (USB).
 * Elle isole la logique matérielle de l'interface graphique en utilisant le système 
 * de Signaux et Slots de Qt.
 */

#ifndef COMMUNICATIONLORA_H
#define COMMUNICATIONLORA_H

#include <QObject>
#include <QSerialPort>

class CommunicationLora : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructeur de la classe CommunicationLora
     * @param parent Pointeur vers l'objet parent Qt (gestion automatique de la mémoire)
     */
    explicit CommunicationLora(QObject *parent = nullptr);
    
    /**
     * @brief Destructeur de la classe CommunicationLora
     * @details Assure la fermeture propre du port série pour éviter les verrous matériels (Locks).
     */
    ~CommunicationLora();

    /**
     * @brief Configure et ouvre le port de communication série
     * @param nomPort Le nom du port matériel (ex: "/dev/ttyACM0" ou "COM3")
     * @return true si le port a été ouvert avec succès, false sinon
     */
    bool ouvrirPort(const QString &nomPort);
    
    /**
     * @brief Envoie la commande d'interrogation RSSI/SNR à la Gateway ESP32
     * @details Transmet le caractère 'm' sur le port série et force le vidage du tampon.
     */
    void demandeRssiSnr();

signals:
    /**
     * @brief Signal émis lorsqu'une ligne de données complète est reçue
     * @param message La chaîne de caractères reçue, nettoyée et prête à être affichée
     */
    void messageRecu(const QString &message);

private slots:
    /**
     * @brief Slot déclenché automatiquement dès que des octets arrivent sur le port série
     * @details Reconstitue les trames fragmentées grâce à un tampon et détecte les fins de ligne (\n).
     */
    void onReadyRead();

private:
    /** Objet Qt gérant l'interface matérielle du port série */
    QSerialPort portComLora;
    /** Tampon (Buffer) de stockage temporaire pour reconstruire les trames morcelées */
    QByteArray m_buffer;
};

#endif // COMMUNICATIONLORA_H
