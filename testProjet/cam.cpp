/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */

#include "cam.h"

Camera::Camera(const unsigned long _frequence, const std::string _indicatif):
    frequence(_frequence),
    indicatif(_indicatif),
    i(0)
{
    // Copie de la mire avec l'indicatif dans ramfs
    std::ostringstream commande;
    commande << "convert -pointsize 12 -fill white -box black -draw \"text 125,32 '" << indicatif;
    commande << "'\" /home/pbs/sstv/mire_320_256.jpg /ramfs/mire.jpg";
    system(commande.str().c_str());

    float fMHz = frequence / 1000000.0;

    commande.str(""); commande.clear();
    commande << "mogrify -pointsize 12 -fill white -undercolor black -gravity northwest ";
    commande << "-annotate +125+201 '" << std::setfill('0') << std::fixed << std::setprecision(3) << fMHz << " MHz' /ramfs/mire.jpg";
    system(commande.str().c_str());

    // Convertion en RGB 8 bits
    system("convert -depth 8 /ramfs/mire.jpg /ramfs/mireRGB.rgb");
}

Camera::Camera(const Camera& orig) {
}

Camera::~Camera() {
}

void Camera::envoyerPhoto(){
    
    // Prise d'une photo en basse résolution
    system("rpicam-still --width 320 --height 256 -o /ramfs/photo.jpg");

    // add texte & date
    std::ostringstream commande;
    commande << "convert -pointsize 20 -box white -draw \"text 10,20 '" << indicatif;
    commande << "  `date +\"%d/%m/%Y %T\"`'\" /ramfs/photo.jpg /ramfs/photo_date.jpg";
    system(commande.str().c_str());

    // Convertion en RGB 8 bits
    system("convert -depth 8 /ramfs/photo_date.jpg /ramfs/photo_date.rgb");

    // Emission SSTV
    commande.str(""); commande.clear();
    commande << "/home/pbs/rpitx/pisstv /ramfs/photo_date.rgb ";
    commande << frequence;
    system(commande.str().c_str());
}

void Camera::envoyerMire(){

    // Emission SSTV
    std::ostringstream commande;
    commande << "/home/pbs/rpitx/pisstv /ramfs/mireRGB.rgb ";
    commande << frequence;
    system(commande.str().c_str());

}

void Camera::enregistrerPhoto(){
    
    std::ostringstream commande;
    commande << "rpicam-still --width 2304 --height 1296 -o /home/pbs/photos/photo_";
    commande << std::setw(3) << std::setfill('0') << i << ".jpg ";
    system(commande.str().c_str());
    

    commande.str(""); commande.clear();
    commande << "convert -pointsize 80 -box white -draw \"text 10,80 '"<< indicatif << "  `date +\"%d/%m/%Y %T\"` ";
    commande << "' \" ";
    commande << "/home/pbs/photos/photo_";
    commande << std::setw(3) << std::setfill('0') << i << ".jpg ";
    commande << "/home/pbs/photos/photo_";
    commande << std::setw(3) << std::setfill('0') << i << ".jpg ";
    system(commande.str().c_str());
    i++;
    
}