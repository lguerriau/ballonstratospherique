# Projet Ballon Stratosphérique (PBS) - TW4
📝 Présentation Générale

Ce projet consiste au développement d'un système autonome pour un ballon stratosphérique capable d'atteindre une altitude de 30 km. Il comprend une nacelle embarquée pour la capture de données scientifiques et d'images, ainsi qu'une station sol pour le suivi en temps réel et l'analyse post-vol.

Le projet est réalisé dans le cadre du BTS CIEL (Cybersécurité, Informatique et réseaux, Électronique) au Lycée Touchard-Washington du Mans.

🎯 Objectifs de la Mission

    Suivi en temps réel : Localisation précise du ballon via les liaisons radio LoRa et APRS.

    Collecte de données scientifiques : Mesure de la température, pression et accélération pour l'étude de l'atmosphère et la vérification de la loi des gaz parfaits.

    Capture d'images : Prise de photos en haute altitude et transmission au sol via le protocole SSTV.

    Récupération de la nacelle : Utilisation d'un dispositif de localisation GPS pour garantir la récupération du matériel après l'atterrissage.

🛠️ Architecture Technique
Matériel Embarqué (Nacelle)

    Unité Centrale : Raspberry Pi Zero W.

    Capteurs :

        Environnemental : BME280 (Pression, Température, Humidité).

        Mouvement : MPU 6050 / HW123 (Accéléromètre 3 axes).

        Température Interne : LM75.

    Positionnement : GPS SIRFstar IV.

    Communication : Module LoRa RA-02 (433 MHz) et Émetteur VHF pour la SSTV.

    Image : Caméra PiCam V3.

    Horodatage : RTC DS3231.

Logiciels et Protocoles

    Langages : C++ (Embarqué et IHM), Python (scripts de traitement), PHP/SQL (Serveur Web).

    Frameworks : Qt 6 pour l'application de contrôle au sol.

    Protocoles Radio :

        LoRa + APRS : Pour la télémétrie courte et longue distance.

        SSTV (Martin M1) : Pour la transmission d'images via ondes VHF.

    Base de données : MariaDB pour l'archivage des trames et événements.

📂 Structure du Code Source

Le dossier PBS/ contient l'application de la station sol développée avec Qt :

    PBS.pro : Fichier de configuration du projet Qt (incluant les modules serialport et widgets).

    communicationlora.h/cpp : Classe gérant la communication série avec la passerelle LoRa (ESP32 TTGO).

    widget.h/cpp : Interface graphique principale pour visualiser la télémétrie et envoyer des requêtes.

    main.cpp : Point d'entrée de l'application.

🚀 Installation et Utilisation

    Environnement : Utilisez l'IDE NetBeans avec le plugin C/C++ (requis par le projet) ou Qt Creator.

    Compilation :

        Ouvrez le fichier PBS.pro.

        Assurez-vous d'avoir Qt 6 installé avec le module Qt Serial Port.

        Compilez le projet en mode Debug ou Release.

    Connexion : Connectez la Gateway LoRa (ESP32) au PC via USB. L'application utilise /dev/ttyACM0 (ou le port COM correspondant) pour communiquer.

👥 Équipe du Projet (Groupe TW4)

    PAIN Iako : Développement du module de télémesure et sauvegarde CSV.

    BRANDS Noé : Interface utilisateur, communication LoRa et détection de chute/atterrissage.

    KALO Harold : Transmission d'images SSTV, serveur Web et traitement vidéo.

    GUERRIAU Lucien : Récupération des données APRS.fi, base de données et cartographie.

Projet soutenu par le CNES et l'association Planète Sciences.
