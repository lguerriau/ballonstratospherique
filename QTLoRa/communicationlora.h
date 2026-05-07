#ifndef COMMUNICATIONLORA_H
#define COMMUNICATIONLORA_H

#include <QObject>
#include <QSerialPort>

class CommunicationLora : public QObject // On hérite de QObject pour les signaux
{
    Q_OBJECT
public:
    explicit CommunicationLora(QObject *parent = nullptr);
    ~CommunicationLora();

    bool ouvrirPort(const QString &nomPort); // Pour ouvrir /dev/ttyACM0
    void demandeRssiSnr();                  // Envoie 'm'

signals:
    // Ce signal sera "émis" vers le Widget quand un message arrive
    void messageRecu(const QString &message);

private slots:
    void onReadyRead(); // Slot interne pour lire le port série

private:
    QSerialPort portComLora;
    QByteArray m_buffer;
};

#endif // COMMUNICATIONLORA_H
