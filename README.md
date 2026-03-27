Projet Ballon Stratosphérique - BTS CIEL 2026

Ce projet, réalisé au Lycée Touchard-Washington (Le Mans) en partenariat avec le CNES et Planète Sciences, consiste à concevoir et mettre en œuvre une nacelle de ballon stratosphérique capable de transmettre des données scientifiques et des images en temps réel.
📋 Présentation du projet

L'objectif principal est de renouveler l'expérience de vérification de la loi des gaz parfaits en mesurant le volume du ballon durant son ascension. Pour cela, une caméra embarquée estime le diamètre du ballon par comparaison avec un réflecteur.

Le système assure également :

    La transmission d'images en direct via le protocole SSTV (Slow Scan Television).

    Le suivi télémétrique et la localisation via les protocoles APRS et LoRa.

    La détection d'événements critiques (éclatement du ballon, atterrissage).

    Un système de requête radio montante pour interroger la nacelle sur la qualité du signal (RSSI/SNR).

🏗️ Architecture du système
1. La Nacelle (Embarqué)

Basée sur un Raspberry Pi Zero, elle regroupe les fonctions de capture et de transmission :

    Caméra PiCam V3 : Prise de photos du ballon et du réflecteur.

    Capteurs : Mesures de température, pression, humidité (BME280) et accélération (MPU6050).

    Module LoRa RA-02 : Transmission de la télémétrie en UHF (433 MHz).

    Émetteur HF (28 MHz) : Transmission des images SSTV modulées en SSB.

2. Station au Sol & Suivi

    Gateway LoRa : Réception des trames de télémétrie et retransmission vers le serveur aprs.fi.

    Récepteur SSTV : Décodage des images reçues via la carte son d'un PC.

    Serveur Web : Affichage en temps réel des photos, de la télémétrie et de la position du ballon par rapport au véhicule de suivi.

    Client LoRa (TTGO ESP32) : Interface permettant d'envoyer des requêtes de diagnostic à la nacelle.

🛠️ Spécifications techniques (Gateway LoRa)

Le dossier LORA_SOL contient le code pour la passerelle de réception au sol utilisant un module TTGO LoRa32 V2.1.

    Fréquence : 433.775 MHz.

    Sécurité : Filtrage des messages par indicatif autorisé (ex: F4KMN-9).

    Commandes supportées : RSSI/SNR pour obtenir les statistiques de réception de la nacelle.

    Gestion des flux : Intègre un mécanisme d'accusé de réception (ACK) avec timeout pour fiabiliser les échanges.

💻 Installation et Développement

    Environnement : Linux Debian Stable.

    IDE : NetBeans (C++) pour le Raspberry Pi et PlatformIO pour les modules ESP32.

    Frameworks : Qt6 pour l'interface de commande utilisateur.

    Documentation : Générée via Doxygen pour l'ensemble du code source.

📅 Calendrier du projet

    Début du projet : Mars 2026.

    Lancement prévu : Lundi 18 mai 2026.

    Remise du dossier final : 29 mai 2026.
