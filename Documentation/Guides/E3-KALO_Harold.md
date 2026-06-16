# Lors de la demo, vous avez accès à mon repertoire perso. Si vous avez la flemme de lancer github et de retrouver mes fichiers, mes fichers concernant le projet ballon se situent dans :
/hkalo/Documents/CIEL2/Projet_2025-26/ 

# Avant la demo :

0) Allumer la station radio : ouvrir vanne (première vanne verte en haut à gauche de la baie), mettre alimentation (générateur noir en bas de la baie) et allumer la radio (bouton violet). Puis avec la roue mettre le récepteur a 29 Mhz (si ce n'est pas déjà le cas). 

1) Allumer la RaspberryPi ( celle où il ya la cameraPi), et mettre l'antenne (le grand fil bleu), sinon la sstv ne marchera pas.

2) Allumer le poste Windows et ouvrir Yoniq.


3) Se connecter au poste Windows avec le poste Linux via le navigateur de fichiers (une demo est mise sur le google docs -> notes PBS où le chemin du dossier partagé est inscrit).

ligne : smb://172.18.58.111/photossstv/, identifiant : admin, mdp : touchard72 comme le poste Windows.


---

# A. Installation 


> [!IMPORTANT]
> A ne pas faire si ces fichiers sont déja installés sur la carte Raspi, ***ce qui est le cas pour l'examin oral final de BTS CIEL*** $\color{red}{\text{(Recompilation pas necessaire, vous pouvez passer a l'étape B)}}$
> Les dossiers de code sont dans la branche 'Harold' et sont déja triés par parties. 
---







# B. Partie Raspberry :

---

1) ouvrir PuTTY, et connecter vous en ssh sur l'adresse de la raspberry (172.18.59.60) ou si vous voulez voir le code, prendre le code concernant la raspberry (dans github ou mes fichiers de mon dossier perso) et l'ouvrir avec NetBeans et éxécuter le code.


2) Dès que vous ếtes dans le terminal, lancer le programme en ecrivant : sudo ./rasperrypi (manip ssh uniquement)


3) Le programme va prendre une photo et l'enregistrer dans la carte SD (Toutes les 30 secondes) et va envoyer la SSTV pendant 5 minutes (attendre entre 1 et 2 minutes avant que la sstv s'envoie, oui c'est long).   


4) Pour vérifier la sstv, il faut entendre le bruit de la station radio et voir yoniq puis le logiciel affiche la photo ligne par ligne. 


5) Les photos Yoniq sont stockées dans l'explorateur de fichiers Windows : OS -> HAM -> MMSSTV -> History 


# C. Partie Importation photos : 

---

1) Prendre le code pour l'importation de photos de la raspi nommé 'importation_photos' et l'ouvrir ou le copier avec QTCreator.


2) Lancer le programme : affiche un widget où faut rentrer le mdp de la raspi (pbs). 

  (l'adresse IP et le dossier local "/home/USERS/ELEVES/CIEL2024/hkalo/sstvYoniq/ARCHIVES_PHOTOS_IMPORTATION sont déjà écrit) et se connecter via le bouton.


3) Dans la fenêtre, les fichiers .jpg de la raspi sont affichées.


4) Puis Cliquez sur importer avec le bouton et les photos seront enregistrées dans le fichier local inscrit.



# D. Partie Watchdog : 

---

1) Prendre le code pour la surveillance de fichiers .bmp (sstv) à transmettre sur le poste Linux nommé 'Watchdog' et l'ouvrir ou le copier avec QTCreator


2) Lancer le programme : affiche un widget vide (c'est normal il faut regarder la console). Toutes les informations de surveillance seront écrites : La détection de nouveau fichiers et la mise du chemin de la photos dans la base de données table IMAGES. 




# E. Partie IHM : 

---
0) > [!IMPORTANT] pour visionner les images dans l'ihm web 

1) Prendre le code pour le visionnage de photos et la géneration de vidéo nommé 'IHMWeb' et l'ouvrir ou le copier avec NETBEANS (Partie Harold) ou Prendre le code dans "**ballonstratospherique/Sol/**", télécharger "**Interface_Internaute**".(Partie intégration) et l'ouvrir avec NETBEANS.

1bis. Vous pouvez inscrire cette URL : http://127.0.0.1:8085/  pour visionner l'ihm dans un autre poste du lycée (faut que le netbeans soit lancer avant).
   

2) Cliquer sur l'onglet 'Galerie SSTV' et les photos seront mises dans l'ordre chronologique. 

3) Pour la vidéo, cliquer sur l'onglet 'Générer la vidéo Timelapse' et la vidéo sera générée. Un bouton 'retour' est présent pour revenir sur la page et un bouton 'actualiser' est présent pour actualiser l'ordre de passage des photos. 



