**Partie transmission de photos ballon** -> sol du projet ballonstratospherique

**Activités avant projet** : phase d'analyse du projet.

**Activités avant projet 2** : phase de conception du projet.

**sstvYoniq** : photos prise par la raspberry stockée par le poste Yoniq (Windows)

**testUnitaire** : fiche de test unitaire sur le module ...



---


**Dans '/Documentation/Realisation/Code_source'**

ihmWeb : interface web pour visionner les photos et réaliser une vidéo à partir de ces photos.

raspberryPi : programmes de la raspberry afin de prendre des photos, emettre les ondes radio (29Mhz), et les sauvegarder sur la carte SD du micro-contrôleur

watchdog : programme pour transferer les photos (via un chien de garde) du poste Windows au poste Linux(serveur et bdd).

importation_photos : programme pour importer des photos en ssh de la raspberryPi



---


**Dans '/Documentation/Realisation/Integration/code_source/phpIntegration'**

phpIntegration : interface web pour visionner les photos et réaliser une vidéo à partir de ces photos ainsi la télémetrie intégrée.
