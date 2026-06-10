**1. Lancer le programme du MPU6050 sur la raspi**

**A. Connection à la Raspi**
Vérifier que la Raspi est branchée

Faire la commande "ssh pbs@172.18.58.86"

Mettre le mot de passe "pbs"

**B. Lancer le projet "mpu6050" :**
A partir de "pbs@PBS:~ $" , faire la commande "cd ./home/Accelerometre" ou clique droit -> accéder au terminal à "/home/Accelerometre"

Lancer le programme avec "sudo ./mpu6050"

Quand le menu est affiché, tappez "1"

après initialisation du capteur, secouez la carte dans tout les sens et faites lui faire un down smash de Kazuya pour qu'il envoie les balises dans l'ordre " En vol, Burst, Landing "
attention l'ordre des balises envoyé est forcément dans cette ordre

Sur "https://aprs.fi/?c=raw&call=F4KMN-9", vérifier que les trames sont bien reçues, connexion obligatoire. si vous n'avez pas de compte ->
Email : f4kmn@yahoo.com
mdp : b112b112

**2. Lancer le programme de l'esp32 (gateway)**

 // à faire
 
**3. Lancer l'ihm**
 
  // à faire
