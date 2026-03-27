#include "communicationlora.h"
#include <QDebug>

// --- Constructeur ---
CommunicationLora::CommunicationLora(QObject *parent) : QObject(parent)
{
    // C'est ici qu'on branche "l'oreille" du port série.
    // Dès que le port reçoit des données (readyRead), il appelle notre fonction (onReadyRead).
    connect(&portComLora, &QSerialPort::readyRead, this, &CommunicationLora::onReadyRead);
}

// --- Destructeur ---
CommunicationLora::~CommunicationLora()
{
    // Sécurité : on s'assure de bien fermer le port quand on quitte le programme
    // pour éviter le fameux bug du "Port occupé" (Lock).
    if (portComLora.isOpen()) {
        portComLora.close();
        qDebug() << "Port série fermé proprement.";
    }
}

// --- Configuration et Ouverture du Port ---
bool CommunicationLora::ouvrirPort(const QString &nomPort)
{
    portComLora.setPortName(nomPort);
    portComLora.setBaudRate(QSerialPort::Baud115200); // Vitesse configurée sur ton ESP32
    portComLora.setDataBits(QSerialPort::Data8);
    portComLora.setParity(QSerialPort::NoParity);
    portComLora.setStopBits(QSerialPort::OneStop);
    portComLora.setFlowControl(QSerialPort::NoFlowControl);

    // Tente d'ouvrir le port en lecture et écriture, et renvoie vrai (true) si ça a marché
    return portComLora.open(QIODevice::ReadWrite);
}

// --- Envoi de la commande à la Gateway ---
void CommunicationLora::demandeRssiSnr()
{
    if (portComLora.isOpen()) {
        portComLora.write("m"); // 'm' est la lettre attendue par ta Gateway
        qDebug() << ">>> Commande 'm' envoyée au module LoRa.";
    } else {
        qDebug() << ">>> ERREUR : Impossible d'envoyer, le port est fermé.";
    }
}

// --- Réception et Traitement des données ---
void CommunicationLora::onReadyRead()
{
    // 1. On ajoute les nouveaux octets au bout du tampon
    m_buffer.append(portComLora.readAll());

    // 2. Tant qu'il y a un retour à la ligne (\n) dans le tampon
    while (m_buffer.contains('\n')) {
        // On trouve la position du premier \n
        int pos = m_buffer.indexOf('\n');

        // On extrait la ligne jusqu'à cette position
        QByteArray lineData = m_buffer.left(pos).trimmed();

        // On supprime la ligne extraite du tampon (pour ne pas la traiter 2 fois)
        m_buffer.remove(0, pos + 1);

        // On transforme en QString
        QString message = QString::fromUtf8(lineData);

        if (!message.isEmpty()) {
            qDebug() << "<<< Message complet reçu :" << message;
            // On envoie enfin la ligne ENTIÈRE au Widget
            emit messageRecu(message);
        }
    }
}
