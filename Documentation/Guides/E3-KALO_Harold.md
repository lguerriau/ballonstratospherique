avant la demo :


- allumer la station radio : ouvrir vanne, mettre alimentation et allumer la radio (bouton violet).

Puis avec la roue mettre le récepteur a 29 Mhz (si ce n'est pas déjà le cas). 


- allumer le poste Windows et ouvrir Yoniq.


- Se connecter au poste Windows avec le poste Linux via le navigateur de fichiers (une demo est mise sur le google docs -> notes PBS où le chemin du dossier partagé

est inscrit).



Partie Raspberry :

- prendre le code pour la prise de photos nommé RaspberryPi et l'ouvrir ou le copier avec NETBEANS.


- lancer le programme : le programme prend une photo toutes les 30 secondes et émet la sstv toutes les 5 minutes.


- pour vérifier la prise de photos, on peut se connecter via ssh (172.18.59.60) sur la RaspberryPi.


- pour vérifier la sstv, il faut voir yoniq et le logiciel affiche la photo ligne par ligne. 


- les photos Yoniq sont stockées dans l'explorateur de fichiers Windows : OS -> HAM -> History 



Partie Importation photos : 

- prendre le code pour l'importation de photos de la raspi nommé 'importation_photos' et l'ouvrir ou le copier avec QTCreator.


- lancer le programme : affiche un widget où faut rentrer le mdp de la raspi (pbs). 

  (l'adresse IP et le dossier local "/home/USERS/ELEVES/CIEL2024/hkalo/sstvYoniq/ARCHIVES_PHOTOS_IMPORTATION et déjà écrit) et se connecter via le bouton.


- Dans la fenêtre, les fichiers .jpg de la raspi sont affichées.


- Puis Cliquez sur importer avec le bouton et les photos seront enregistrées dans le fichier local inscrit.


Partie Watchdog : 

- prendre le code pour la surveillance de fichiers .bmp (sstv) à transmettre sur le poste Linux nommé 'Watchdog' et l'ouvrir ou le copier avec QTCreator


- lancer le programme : affiche un widget vide (c'est normal il faut regarder la console). Toutes les informations de surveillance seront écrites

  pour la détection de nouveau fichiers et la mise du chemin de la photos dans la base de données table IMAGES. 


Partie IHM : 

- prendre le code pour le visionnage de photos et la géneration de vidéo nommé 'IHMWeb' et l'ouvrir ou le copier avec NETBEANS.

- cliquer sur l'onglet 'Galerie SSTV' et les photos seront mises dans l'ordre chronologique. 

- pour la vidéo, cliquer sur l'onglet 'Générer la vidéo Timelapse' et la vidéo sera générer. Un bouton retour est présent pour revenir sur la page et actualiser est présent pour actualiser l'ordre de passage des photos. 



