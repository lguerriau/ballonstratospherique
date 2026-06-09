/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */

/**
   @file cam.cpp
   @brief Implémentation de la classe Camera
   @version 1.0
   @author Harold KALO
   @date 26/05/2026
   @details Classe modélisant la prise de capture photos de la raspberry et la transmission en sstv vers une station radio et un poste au sol
*/

#include "cam.h"
#include <iostream>
#include <cstdio>  
#include <cstdlib> 
#include <cstring> 
#include <cassert>

/**
 * @brief Camera::Camera
 * @param _frequence fréquence d'émission modulée
 * @param _indicatif indicatif station radio
 * @author Harold KALO
 */
Camera::Camera(const unsigned long _frequence, const char* _indicatif) :
frequence(_frequence) {
    
    assert(_frequence > 0);
    assert(_indicatif != nullptr);
    
    this->nbPhotos = 0;

    std::strncpy(this->indicatif, _indicatif, sizeof(this->indicatif) - 1);
    this->indicatif[sizeof(this->indicatif) - 1] = '\0';

    char commande[256];
    
    int retour_snprintf = std::snprintf(commande, sizeof(commande),
        "convert -pointsize 12 -fill white -box black -draw \"text 125,32 '%s'\" /home/pbs/sstv/mire_320_256.jpg /ramfs/mire.jpg",
        this->indicatif);
        
    assert(retour_snprintf > 0 && retour_snprintf < (int)sizeof(commande));

    int res = std::system(commande);
    if (res != 0) {
        std::cerr << "Erreur critique d'initialisation de la mire" << std::endl;
    }

    res = std::system("convert -depth 8 /ramfs/mire.jpg /ramfs/mireRGB.rgb");
    if (res != 0) {
        std::cerr << "Erreur conversion mire RGB" << std::endl;
    }
}

/**
 * @brief Constructeur de recopie Camera::Camera
 * @param orig Référence constante vers l'objet Camera d'origine à copier.
 * @author Harold KALO
 */
Camera::Camera(const Camera& orig) {
    assert(orig.frequence > 0);
    this->frequence = orig.frequence;
    this->nbPhotos = orig.nbPhotos;
    std::strncpy(this->indicatif, orig.indicatif, sizeof(this->indicatif));
}

/**
 * @brief Camera::~Camera
 * @author Harold KALO
 */
Camera::~Camera() {
    // Règle 5: Assertion pour prouver l'état final sain
    assert(this->nbPhotos >= 0);
}

/**
 * @brief Camera::envoyerPhoto
 * @author Harold KALO
 */
void Camera::envoyerPhoto() {
    
    assert(this->frequence >= 29000000); 
    assert(this->nbPhotos > 0); 

    char convert[256];
    char commande[256];

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

/**
 * @brief Camera::envoyerMire
 * @author Harold KALO
 */
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

/**
 * @brief Camera::enregistrerPhoto
 * @author Harold KALO
 */
void Camera::enregistrerPhoto() {
  
    assert(this->nbPhotos >= 0);
    assert(this->nbPhotos < 1000); 

    char nomFinal[128];
    char commande[256];

    int ret = std::snprintf(nomFinal, sizeof(nomFinal), "/home/pbs/photos/photo_%03d.jpg", this->nbPhotos);
    assert(ret > 0 && ret < (int)sizeof(nomFinal));

    ret = std::snprintf(commande, sizeof(commande), "sudo rpicam-still -n -t 500 -o %s", nomFinal);
    assert(ret > 0 && ret < (int)sizeof(commande));

    std::cout << "Capture HD : " << nomFinal << std::endl;

    
    if (std::system(commande) == 0) {
        char commandeAnnotation[512];
        
        ret = std::snprintf(commandeAnnotation, sizeof(commandeAnnotation),
            "sudo convert %s -gravity North -pointsize 250 -fill red -undercolor white -annotate +0+50 \" %s $(date +'%%d/%%m/%%y %%H:%%M:%%S') \" %s",
            nomFinal, this->indicatif, nomFinal);
            
        assert(ret > 0 && ret < (int)sizeof(commandeAnnotation));

        std::cout << "Annotation appliquée pour la SSTV." << std::endl;
        
        if (std::system(commandeAnnotation) == 0) {
            this->nbPhotos++;
        }
    }
}
