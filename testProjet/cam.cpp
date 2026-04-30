/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */

#include "cam.h"
#include <iomanip>
#include <sstream>
#include <unistd.h>

Camera::Camera(const unsigned long _frequence, const std::string _indicatif) :
frequence(_frequence),
indicatif(_indicatif) {
    this->nbPhotos = 0;

    // Copie de la mire avec l'indicatif dans ramfs
    std::ostringstream commande;
    commande << "convert -pointsize 12 -fill white -box black -draw \"text 125,32 '" << indicatif;
    commande << "'\" /home/pbs/sstv/mire_320_256.jpg /ramfs/mire.jpg";
    system(commande.str().c_str());

    // Convertion en RGB 8 bits
    system("convert -depth 8 /ramfs/mire.jpg /ramfs/mireRGB.rgb");
}

Camera::Camera(const Camera& orig) {
}

Camera::~Camera() {
}

void Camera::envoyerPhoto() {
    // 1. Préparation d'une version basse résolution pour la radio (320x256)
    // On prend la dernière photo HD pour l'envoyer
    std::ostringstream convert;
    convert << "convert /home/pbs/photos/photo_" << std::setw(3) << std::setfill('0') << (nbPhotos - 1)
            << ".jpg -resize 320x256! -depth 8 /ramfs/radio.rgb";
    system(convert.str().c_str());

    // 2. Emission SSTV via rpitx
    std::ostringstream commande;
    commande << "sudo /home/pbs/rpitx/pisstv /ramfs/radio.rgb " << frequence;
    std::cout << "Emission SSTV en cours..." << std::endl;
    system(commande.str().c_str());
}

void Camera::envoyerMire() {

    std::ostringstream commande;
    commande << "sudo /home/pbs/rpitx/pisstv /ramfs/mireRGB.rgb " << frequence;
    system(commande.str().c_str());

}

void Camera::enregistrerPhoto() {

    std::ostringstream nomFinal;
    std::ostringstream commande;

    // Nom de fichier incrémenté (ex: photo_001.jpg)
    nomFinal << "/home/pbs/photos/photo_" << std::setw(3) << std::setfill('0') << nbPhotos << ".jpg";

    // 1. Prise de la photo HD
    // On utilise -n pour éviter d'ouvrir une fenêtre sur le bureau de la Raspberry
    commande << "sudo rpicam-still -n -t 500 -o " << nomFinal.str();
    std::cout << "Capture HD : " << nomFinal.str() << std::endl;

    if (system(commande.str().c_str()) == 0) {
        // 2. Annotation (Date et Indicatif) directement sur l'image
        std::ostringstream commandeAnnotation;
        commandeAnnotation << "convert " << nomFinal.str()
                << " -pointsize 80 -fill red -draw \"text 10,80 '" << indicatif
                << "  `date +\"%d/%m/%Y %T\"` '\" " << nomFinal.str();
        system(commandeAnnotation.str().c_str());

        std::cout << "Succès : Photo sauvegardée sur SD." << std::endl;
        nbPhotos++; // On prépare l'index pour la suivante
    }
}

