# Installation 


> [!IMPORTANT]
> A ne pas faire si ces fichiers sont déja installés sur la carte Raspi, ***ce qui est le cas pour l'examin oral final de BTS CIEL*** $\color{red}{\text{(Recompilation pas necessaire, vous pouvez passer a l'étape "Connection a la Raspi")}}$ 
---

0) *Vérifier que la Raspi est branchée*

1) Dans "**ballonstratospherique/Nacelle/**", télécharger "**BME280**", "**LM75**", "**RA02**"

2) Dans "**ballonstratospherique/Sol/**", télécharger "**Interface_Internaute**"

3) Mettre tous ces fichiers dans le dossier "**NetBeansProjects**" local a votre PC

4) Executer "**BME280**" puis "**LM75**" puis "**RA02**" en remote sur la Raspi

# Connection à la Raspi

---

0) *Vérifier que la Raspi est branchée*

1) Faire la commande "**ssh pbs@172.18.59.66**"

2) Mettre le mot de passe "**pbs**"

# Récupérer la télémétrie

---

1) Lancer le projet "**RA02**" :

- A partir de "**pbs@PBS:~ $**" , faire la commande "**cd ./.netbeans/remote/172.18.59.66/b106tu4p2-Linux-x86_64/home/USERS/ELEVES/CIEL2024/ipain/NetBeansProjects/RA02/dist/Debug/GNU-Linux/**"

- Lancer le programme avec "**sudo ./ra02**"

2) Quand le menu est affiché, tappez "**1**"

3) Sur "**https://aprs.fi/?c=raw&call=F4KMN-9**", vérifier que les trames sont bien reçues

# Visualiser la télémétrie

---

1) Lancer le projet "**Interface_Internaute**" (aka "**test_ihm**") pour observer la mise a jour en toutes les 10 minutes des données télémétriques (limite API)

2) Pour plus de données , un fichier "**telemetrie_exemple.json**" est fournis, il suffit de l'importer
