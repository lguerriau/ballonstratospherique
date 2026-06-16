# A. Installation 


> [!IMPORTANT]
> A ne pas faire si ces fichiers sont déja installés sur la carte Raspi, ***ce qui est le cas pour l'examin oral final de BTS CIEL*** $\color{red}{\text{(Recompilation pas necessaire, vous pouvez passer a l'étape "B. Connection a la Raspi")}}$ 
---


> [!TIP]
> Il faut télécharger les fichiers suivnats depuis ***GitHub*** grâce à : https://download-directory.github.io/

1. Dans "**ballonstratospherique/Nacelle/**", télécharger "**BME280**", "**LM75**", "**RA02**"

2. Dans "**ballonstratospherique/Sol/**", télécharger "**Interface_Internaute**"

3. Mettre tous les dossiers dans le dossier "**NetBeansProjects**" local a votre PC

# B. Connection à la Raspi

---

> [!TIP]
> *Vérifier que la Raspi est branchée*

1) Faire la commande "**ssh pbs@172.18.59.66**"

2) Mettre le mot de passe "**pbs**"

# C. Obtention des grandeurs physiques

---

1) Lancer le projet "**RA02**" :

- A partir de "**pbs@PBS:~ $**" , faire la commande "**cd ./.netbeans/remote/172.18.59.66/b106tu4p2-Linux-x86_64/home/USERS/ELEVES/CIEL2024/ipain/NetBeansProjects/RA02/dist/Debug/GNU-Linux/**"

- Lancer le programme avec "**sudo ./ra02**"

2) Quand le menu est affiché, tappez "**1**"

3) Sur "**https://aprs.fi/?c=raw&call=F4KMN-9**", vérifier que les trames sont bien reçues

# D. Visualiser la télémétrie et vérifier la Loi des Gaz Parfaits

---
1. Aller sur le site : http://172.18.58.85/visuBallon/index.php

> [!NOTE]
> Si le site ne fonctionne pas, faire l'étape A2 et A3, puis :
> Lancer le projet "**Interface_Internaute**" (aka "**test_ihm**") avec ***NetBeans*** pour observer la mise a jour en toutes les 10 minutes des données télémétriques (limite API)

> [!CAUTION]
> L'iHM peut ne pas se lancer du premier coup, dans ce cas là il faut réessayer et ignorer toute erreur affichée

2) Pour plus de données , un fichier "**telemetrie_exemple.json**" est fournis *directement dans le dossier du projet*, il suffit de l'importer

3) Pour vérifier la Loi des Gaz Parfaits, cliquez sur le bouton en haut a droite de la page
     - Des aides sont disponibles pour remplir les champs vides
  
     - Si besoin voici des valeurs pour lesquelles la loi fonctionne : 100000Pa, 7.24m³, 289mol, 8.314, 300.95K
     - Ne fonctionne pas : 100000Pa, 2.24m³, 289mol, 8.314, 300.95K
