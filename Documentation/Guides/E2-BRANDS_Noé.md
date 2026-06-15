**1. Lancer le programme du MPU6050 sur la raspi**

**A. Connection à la Raspi**
Vérifier que la Raspi est branchée

Faire la commande "ssh pbs@172.18.58.86"

Mettre le mot de passe "pbs"

**B. Lancer le projet "mpu6050" :**
A partir de "pbs@PBS:~ $" , faire la commande "cd ./home/Accelerometre" ou clique droit -> accéder au terminal à "/home/AccelerometreVOL"

Lancer le programme avec "sudo ./mpu6050"

Quand le menu est affiché, tappez "1"

après initialisation du capteur, secouez la carte dans tout les sens et faites lui faire un down smash de Kazuya pour qu'il envoie les balises dans l'ordre " En vol, Burst, Landing "
attention l'ordre des balises envoyé est forcément dans cette ordre

**2. Lancer la gateway ESP32**

Brancher en USB les 2 esp32 sur votre poste.

Pas besoin de run le projet il est déjà dessus et actif dès le branchement.

Si jamais le jury demande de voir les packets qui transitent ( ce qui n'arrivera pas )
allez sur Putty et mettez les valeurs suivantes pour ouvrir l'ESP32 :
SERIAL
SERIAL LINE ( en haut ) : /dev/ttyACM0 ( pour le premier ) /dev/ttyACM1 ( pour le deuxième )
SPEED : 115200 ( normalement ok )
puis confirmez, vous verrez une fenetre, il est possible d'appuyer sur la touche "m" pour envoyer un message.
L'esp32 ne sert que pour la partie 3 sur QT.

 
**3. Lancer l'ihm**
 
Ouvrir QT sur votre session puis ouvrir projet qui se situt dans /home/USERS/ELEVES/CIEL2024/nbrands/ProjetQT/QTLORA
en cliquant sur le .pro

Si Qt n'est pas content, acceptez, puis refaite la manipe. ( cela peut inclure de configurer le projet, juste faite acceptez ou ok). 

**Si le Qlabel est rouge ** alors vous n'avez pas brancher l'esp32, branchez le et relancer le programme.



**Conclusion**
Lorsque que tout est en place vous pouvez montrer le projet au jury en faisant :
1 - balancer la Raspi dans tout les sens, ça envoie la requete lora.
2 - constater que l'esp32 via putty recoit bien l'état du vol.
3 -  constater que les 2 esp32 communique en appuyant sur "Emettre la requete" sur QT, réponse uniquement en ligne de debug ( réponse de type -> RSSI=XX.X|SNR=XX )

