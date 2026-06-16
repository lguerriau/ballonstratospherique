/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/**
   @file cam.h
   @brief Déclaration de la classe Camera
   @version 1.0
   @author Harold KALO
   @date 26/05/2026
*/

#ifndef CAM_H
#define CAM_H

#include <string>

class Camera
{
private:
    /** frequence d'émission modulée pour la transmission sstv*/
    unsigned long frequence;
    
    /** indicatif de la station radio*/
    char indicatif[16];
    
    /** compteur de photos */
    int nbPhotos;

public:

    Camera(const unsigned long _frequence, const char* _indicatif);
    Camera(const Camera& orig);
    virtual ~Camera();

    void envoyerPhoto();
    void envoyerMire();
    void enregistrerPhoto();


};



#endif /* CAM_H */

