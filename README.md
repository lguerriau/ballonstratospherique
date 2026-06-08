# Projet Ballon Stratosphérique (PBS) - TW4
📝 Présentation Générale

Ce projet consiste au développement d'un système autonome pour un ballon stratosphérique capable d'atteindre une altitude de 30 km. Il comprend une nacelle embarquée pour la capture de données scientifiques et d'images, ainsi qu'une station sol pour le suivi en temps réel et l'analyse post-vol.

Le projet est réalisé dans le cadre du BTS CIEL (Cybersécurité, Informatique et réseaux, Électronique) au Lycée Touchard-Washington du Mans.
___
🎯 Objectifs de la Mission
-
Suivi en temps réel :               Localisation précise du ballon via les liaisons radio LoRa et APRS

Collecte de données scientifiques : Mesure de la température, pression et accélération pour l'étude de l'atmosphère et                                     la vérification de la loi des gaz parfaits

Capture d'images :                  Prise de photos en haute altitude et transmission au sol via le protocole SSTV

Récupération de la nacelle :        Utilisation d'un dispositif de localisation GPS pour garantir la récupération du                                       matériel après l'atterrissage
___
📂️ Structure du projet
-
*Documentation* : 
- Contiens toutes les documentations et guides utiles aux programmes

*Nacelle* :
- BME280 : Programme permettant la mesure des valeurs télémétrique de température, humidité et pression atmosphérique
- LM75 : Programme premettant la mesure de la température interne
- RA02 : Programme d'envoi des données télémétrique, trame APRS Weather

*Sol* :
- Interface_Internaute : Interface de visualisation des données télémétriques et photos SSTV
- Interface_Opérateur : Interface de communication RSSI avec la Nacelle
- Reception : Programme de reception RSSI
- BDD : Contiens ce qui est relatif a la Base de Données
___
🛠️ Architecture Technique
-
🛰️ **Matériel Embarqué (Nacelle)** 

Unité Centrale : Raspberry Pi Zero W

Composants :
- Environnemental : BME280 (Pression, Température, Humidité)
- Température : LM75
- Mouvement : MPU 6050 / HW123 (Accéléromètre 3 axes)
- Communication : Module LoRa RA-02 (433 MHz) et Émetteur VHF pour la SSTV
- Image : Caméra PiCam V3
- Horodatage : RTC DS3231

🔏️ **Logiciels et Protocoles**

Langages : 
- C++ (Embarqué et IHMs)
- Python (scripts de traitement)
- PHP/SQL (Serveur Web)

Frameworks : 
- Qt 6 pour l'application Opérateur de contrôle au sol
- NetBeans pour l'interface de visualisation télémétrique et SSTV
  
Protocoles Radio :
- APRS Weather pour la télémétrie courte et longue distance
- SSTV (Martin M1) pour la transmission d'images via ondes VHF

Base de données : 
- MariaDB pour l'archivage des trames et événements

___

👥️ Équipe du Projet (Groupe TW4)
-
*PAIN Iako* : 
- Développement du module de télémesure
- Communication APRS Weather
- Sauvegarde .json sur la Nacelle
- Interface de visualisation télémétrique en direct

*BRANDS Noé* : 
- Interface utilisateur permettant l'envoi et la réception de requête
- Communication LoRa/Requêtes
- Détection de chute/atterrissage

*KALO Harold* : 
- Transmission d'images SSTV
- Serveur Web
- Traitement vidéo

*GUERRIAU Lucien* : 
- Récupération des données APRS.fi
- Base de données
- Cartographie
  
Projet soutenu par le CNES et l'association Planète Sciences.
[2026]
