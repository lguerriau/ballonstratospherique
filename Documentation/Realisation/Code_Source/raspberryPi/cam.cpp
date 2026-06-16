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

    if (_frequence == 0) {
        std::cerr << " La fréquence d'émission ne peut pas être égale à 0 !" << std::endl;
        this->frequence = 144500000;
    }

    if (_indicatif == nullptr) {
        std::cerr << " L'indicatif fourni est un pointeur nul !" << std::endl;
        _indicatif = "UNKNOWN";
    }

    this->nbPhotos = 0;

    std::strncpy(this->indicatif, _indicatif, sizeof (this->indicatif) - 1);
    this->indicatif[sizeof (this->indicatif) - 1] = '\0';

    char commande[256];

    int retour_snprintf = std::snprintf(commande, sizeof (commande),
            "convert -pointsize 12 -fill white -box black -draw \"text 125,32 '%s'\" /home/pbs/sstv/mire_320_256.jpg /ramfs/mire.jpg",
            this->indicatif);

    if (retour_snprintf <= 0 || retour_snprintf >= (int) sizeof (commande)) {
        std::cerr << "Commande de création de mire trop longue pour le buffer." << std::endl;
        return;
    }

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
    if (orig.frequence == 0) {
        std::cerr << "Recopie d'une caméra avec une fréquence invalide." << std::endl;
    }

    this->frequence = orig.frequence;
    this->nbPhotos = orig.nbPhotos;
    std::strncpy(this->indicatif, orig.indicatif, sizeof (this->indicatif));
}

/**
 * @brief Camera::~Camera
 * @author Harold KALO
 */
Camera::~Camera() {

    if (this->nbPhotos < 0) {
        std::cerr << "Nombre de photos négatif détecté à la destruction !" << std::endl;
    }
}

/**
 * @brief Camera::envoyerPhoto
 * @author Harold KALO
 */
void Camera::envoyerPhoto() {

    if (this->frequence < 29000000) {
        std::cerr << "Fréquence radio trop basse hors limites SSTV." << std::endl;
        return;
    }

    if (this->nbPhotos <= 0) {
        std::cerr << "Aucune photo disponible à envoyer." << std::endl;
        return;
    }

    char convert[256];
    char commande[256];

    int indexPhoto = this->nbPhotos - 1;
    if (indexPhoto < 0) {
        std::cerr << "Index de photo invalide." << std::endl;
        return;
    }

    int ret = std::snprintf(convert, sizeof (convert),
            "convert /home/pbs/photos/photo_%03d.jpg -resize 320x256! -depth 8 /ramfs/radio.rgb",
            indexPhoto);

    if (ret <= 0 || ret >= (int) sizeof (convert)) {
        std::cerr << "Buffer convert trop petit." << std::endl;
        return;
    }

    if (std::system(convert) == 0) {
        ret = std::snprintf(commande, sizeof (commande),
                "sudo /home/pbs/rpitx/pisstv /ramfs/radio.rgb %lu",
                this->frequence);
        if (ret <= 0 || ret >= (int) sizeof (commande)) {
            std::cerr << 'Buffer commande émission trop petit." << std::endl;
            return;
        }

        std::cout << "Emission SSTV en cours..." << std::endl;

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
    if (this->frequence == 0) {
        std::cerr << "Impossible d'envoyer la mire, fréquence non configurée." << std::endl;
        return;
    }

    char commande[256];
    int ret = std::snprintf(commande, sizeof (commande),
            "sudo /home/pbs/rpitx/pisstv /ramfs/mireRGB.rgb %lu",
            this->frequence);

    if (ret <= 0 || ret >= (int) sizeof (commande)) {
        std::cerr << "Buffer de commande Mire trop petit." << std::endl;
        return;
    }

    if (std::system(commande) != 0) {
        std::cerr << "Erreur émission Mire" << std::endl;
    }
}

/**
 * @brief Camera::enregistrerPhoto
 * @author Harold KALO
 */
void Camera::enregistrerPhoto() {

    if (this->nbPhotos < 0 || this->nbPhotos >= 1000) {
        std::cerr << "Nombre maximal de photos (1000) atteint pour la carte SD !" << std::endl;
        return;
    }

    char nomFinal[128];
    char commande[256];

    int ret = std::snprintf(nomFinal, sizeof (nomFinal), "/home/pbs/photos/photo_%03d.jpg", this->nbPhotos);
    if (ret <= 0 || ret >= (int) sizeof (nomFinal)) {
        std::cerr << "Nom de fichier image trop long." << std::endl;
        return;
    }

    ret = std::snprintf(commande, sizeof (commande), "sudo rpicam-still -n -t 500 -o %s", nomFinal);
    if (ret <= 0 || ret >= (int) sizeof (commande)) {
        std::cerr << "Ligne de commande de capture trop longue." << std::endl;
        return;
    }

    std::cout << "Capture HD : " << nomFinal << std::endl;


    if (std::system(commande) == 0) {
        char commandeAnnotation[512];

        ret = std::snprintf(commandeAnnotation, sizeof (commandeAnnotation),
                "sudo convert %s -gravity North -pointsize 250 -fill red -undercolor white -annotate +0+50 \" %s $(date +'%%d/%%m/%%y %%H:%%M:%%S') \" %s",
                nomFinal, this->indicatif, nomFinal);

        if (ret <= 0 || ret >= (int)sizeof(commandeAnnotation)) {
            std::cerr << "Ligne de commande de traitement d'image trop longue." << std::endl;
            return;
        }

        std::cout << "Annotation appliquée pour la SSTV." << std::endl;

        if (std::system(commandeAnnotation) == 0) {
            this->nbPhotos++;
        }
    }
}
