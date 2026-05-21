/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */

#include "cam.h"
#include <iostream>
#include <cstdio>  
#include <cstdlib> 
#include <cstring> 
#include <cassert>

Camera::Camera(const unsigned long _frequence, const char* _indicatif) :
frequence(_frequence) {
    
    assert(_frequence > 0);
    assert(_indicatif != nullptr);
    
    this->nbPhotos = 0;

 // Règle 3: Copie sécurisée sans allocation dynamique
    std::strncpy(this->indicatif, _indicatif, sizeof(this->indicatif) - 1);
    this->indicatif[sizeof(this->indicatif) - 1] = '\0';

    char commande[256];
    
    // Règle 3: Construction de la chaîne avec une taille fixe garantie
    int retour_snprintf = std::snprintf(commande, sizeof(commande),
        "convert -pointsize 12 -fill white -box black -draw \"text 125,32 '%s'\" /home/pbs/sstv/mire_320_256.jpg /ramfs/mire.jpg",
        this->indicatif);
        
    // Règle 5 (Bis): L'assertion valide le bon formatage de la commande
    assert(retour_snprintf > 0 && retour_snprintf < (int)sizeof(commande));

    // Règle 7: Vérification systématique de la valeur de retour
    int res = std::system(commande);
    if (res != 0) {
        std::cerr << "Erreur critique d'initialisation de la mire" << std::endl;
    }

    res = std::system("convert -depth 8 /ramfs/mire.jpg /ramfs/mireRGB.rgb");
    if (res != 0) {
        std::cerr << "Erreur conversion mire RGB" << std::endl;
    }
}

Camera::Camera(const Camera& orig) {
    assert(orig.frequence > 0); // Règle 5
    this->frequence = orig.frequence;
    this->nbPhotos = orig.nbPhotos;
    std::strncpy(this->indicatif, orig.indicatif, sizeof(this->indicatif));
}

Camera::~Camera() {
    // Règle 5: Assertion pour prouver l'état final sain
    assert(this->nbPhotos >= 0);
}

void Camera::envoyerPhoto() {
    // Règle 5: Deux assertions pour valider l'état avant émission
    assert(this->frequence >= 29000000); 
    assert(this->nbPhotos > 0); 

    char convert[256];
    char commande[256];

    // Règle 3 & 6: Portée minimale et pas d'allocation
    int indexPhoto = this->nbPhotos - 1;
    assert(indexPhoto >= 0);

    int ret = std::snprintf(convert, sizeof(convert),
        "convert /home/pbs/photos/photo_%03d.jpg -resize 320x256! -depth 8 /ramfs/radio.rgb",
        indexPhoto);
    assert(ret > 0 && ret < (int)sizeof(convert));

    if (std::system(convert) == 0) {
        ret = std::snprintf(commande, sizeof(commande),
            "sudo /home/pbs/rpitx/pisstv /ramfs/radio.rgb %lu",
            this->frequence);
        assert(ret > 0 && ret < (int)sizeof(commande));
        
        std::cout << "Emission SSTV en cours..." << std::endl;
        
        // Règle 7: On vérifie le retour de l'émetteur radio
        if (std::system(commande) != 0) {
            std::cerr << "Échec de l'émission radio" << std::endl;
        }
    }
}

void Camera::envoyerMire() {
    assert(this->frequence > 0); // Règle 5
    
    char commande[256];
    int ret = std::snprintf(commande, sizeof(commande),
        "sudo /home/pbs/rpitx/pisstv /ramfs/mireRGB.rgb %lu",
        this->frequence);
    
    assert(ret > 0 && ret < (int)sizeof(commande)); // Règle 5

    if (std::system(commande) != 0) { // Règle 7
        std::cerr << "Erreur émission Mire" << std::endl;
    }
}

void Camera::enregistrerPhoto() {
    // Règle 5: Garantir que le compteur n'est pas corrompu avant capture
    assert(this->nbPhotos >= 0);
    assert(this->nbPhotos < 1000); 

    char nomFinal[128];
    char commande[256];

    int ret = std::snprintf(nomFinal, sizeof(nomFinal), "/home/pbs/photos/photo_%03d.jpg", this->nbPhotos);
    assert(ret > 0 && ret < (int)sizeof(nomFinal));

    ret = std::snprintf(commande, sizeof(commande), "sudo rpicam-still -n -t 500 -o %s", nomFinal);
    assert(ret > 0 && ret < (int)sizeof(commande));

    std::cout << "Capture HD : " << nomFinal << std::endl;

    // Règle 7: Vérification impérative du succès matériel de la caméra
    if (std::system(commande) == 0) {
        char commandeAnnotation[512];
        
        ret = std::snprintf(commandeAnnotation, sizeof(commandeAnnotation),
            "sudo convert %s -gravity North -pointsize 250 -fill red -undercolor white -annotate +0+50 \" %s $(date +'%%d/%%m/%%y %%H:%%M:%%S') \" %s",
            nomFinal, this->indicatif, nomFinal);
            
        assert(ret > 0 && ret < (int)sizeof(commandeAnnotation));

        std::cout << "Annotation appliquée pour la SSTV." << std::endl;
        
        if (std::system(commandeAnnotation) == 0) {
            this->nbPhotos++;
            this->envoyerPhoto();
        }
    }
}
