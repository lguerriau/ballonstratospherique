# Avant la demo :

0) Brancher les 2 esp32 sur les ports USB du pc que vous utilisez.
1) Brancher sur une prise secteur la raspi avec une antenne noir et une carte bleu à coté.
2) Allumer votre pc aussi ça serais bien.

# 1. Lancer le programme du MPU6050 sur la raspi

**A. Connection à la Raspi**
1) Vérifier que la Raspi est branchée

2) Faire la commande "ssh pbs@172.18.58.86" sur votre terminal
   
   **OU**

2. Allez dans fichier, fichier, se connecter à un serveur en mettant 172.18.58.86 ainsi que le mot de passe SANS OUBLIEZ d'activer le ssh.


**B. Lancer le projet "mpu6050" :**
A partir de "pbs@PBS:~ $" , faire la commande "cd ./home/Accelerometre" ou clique droit -> accéder au terminal à "/home/AccelerometreVOL"

> [!IMPORTANT]
> Ne pas ouvrir AccelerometrePING il n'est pas stable et en cours de réalisation.
---


Lancer le programme avec "sudo ./mpu6050"

Quand le menu est affiché, tappez "1"

après initialisation du capteur, secouez la carte dans tout les sens et faites lui faire un down smash de Kazuya pour qu'il envoie les balises dans l'ordre " En vol, Burst, Landing "
attention l'ordre des balises envoyé est forcément dans cette ordre

# 2. Lancer la gateway ESP32

1) Brancher en USB les 2 esp32 sur votre poste.

Pas besoin de run le projet il est déjà dessus et actif dès le branchement.

Si jamais le jury demande de voir les packets qui transitent ( ce qui n'arrivera pas )
allez sur Putty et mettez les valeurs suivantes pour ouvrir l'ESP32 :

> SERIAL
> SERIAL LINE ( en haut ) :
> /dev/ttyACM0 ( pour le premier ) /dev/ttyACM1 ( pour le deuxième )
> SPEED : 115200 ( normalement ok )

puis confirmez, vous verrez une fenetre, il est possible d'appuyer sur la touche "m" pour envoyer un message.

L'esp32 ne sert que pour la partie 3 sur QT.

# 3. Lancer l'ihm
 
Ouvrir QT sur votre session puis ouvrir projet qui se situt dans /home/USERS/ELEVES/CIEL2024/nbrands/ProjetQT/QTLORA
en cliquant sur le .pro

Si Qt n'est pas content, acceptez, puis refaite la manipe. ( cela peut inclure de configurer le projet, juste faite acceptez ou ok). 

**Si le Qlabel est rouge**: alors vous n'avez pas brancher l'esp32, branchez le et relancer le programme.

Dans l'ihm il est possible de :
1) Emettre une requete et recevoir la réponse environ après 4 secondes.
2) Recevoir Les différents états de vol dans le tableau et en haut dans le QLabel

# Test complet :

---

> [!IMPORTANT]
> La suite d'étapes est faisable si l'interface Qt fonctionne et si tout est bien branché

1) Lancer le programme **./mpu6050** sur la raspi, puis faite la commencer à monter.
2) Le lora s'envoie et s'affiche sur QT
3) testez tout les flags en agitant la raspi
4) après le flag LANDING, testez l'emission de requetes en appuyant sur Emettre Requête.

Toute ma présentation s'arrête ici


