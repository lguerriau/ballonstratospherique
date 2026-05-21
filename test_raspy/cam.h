/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   cam.h
 * Author: hkalo
 *
 * Created on 26 mars 2026, 08:46
 */

#ifndef CAM_H
#define CAM_H

#include <string>


class Camera
{
private:
    unsigned long frequence;
    char indicatif[16];
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

