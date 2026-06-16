# A. Base de Donnée 
---

> [!TIP]
> *Vérifier que vous êtes bien sur le réseau du lycée*

1) Faire la commande "**ssh -X root@172.18.58.85**" pour se connecter à la VirtualMachine.

2) Mettre le mot de passe "**toto**"

3) Faire la commande "**mariadb -u root -p**" pour se connecter à la base de donnée.

4) Mettre le mot de passe "**toto**"

5) Faire la commande "**USE ballon2026;**" pour se connecter à la bonne base.

5) Effectuer la commande "**TRUNCATE TABLE POSITIONS;**" afin déviter tout problème sur les points au niveau de la carte OSM.

# B. QT Creator

---

1) Lancer "**QT Creator**" :

- Lancer le projet "**RecupPositionsAPRS**" avec ce chemin : ***/home/USERS/ELEVES/CIEL2024/lguerriau/ProjetPBS/Projet Fini/RecupPositionsAPRS*** en cliquant sur le .pro

- Vérifier bien que dans le "**config.ini**", les indicatifs F4KMN-9 et F4KMN-10 sont ciblés

- Lancer le programme et appuyer sur le bouton "**Lancer le serveur**"

# C. Simulation d'une position

---
1) Lancer "**Thonny**" :

- Lancer le projet "**aprs_send_test.py**" avec ce chemin : ***/home/USERS/ELEVES/CIEL2024/lguerriau/ProjetPBS/Projet Fini/aprs_send_test.py***.

- Lancer le programme.

# D. Visualisation des positions

---

1) Lancer "**NetBeans**" :

- Lancer le projet "**OSMPBS**" avec ce chemin : ***/home/USERS/ELEVES/CIEL2024/lguerriau/ProjetPBS/Projet Fini/OSMPBS***

- Lancer le site web.

2) Aller sur QT Creator

- Appuyer sur "**Forcer une requête API**"

> [!TIP]
> Si les positions ne s'affichent pas, recharger le site avec "**F5**" et rappuyer sur "**Forcer une requête API**".
