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

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sstream>
#include <string>

class Camera {
public:
    
    Camera(const unsigned long _frequence, const std::string _indicatif);
    Camera(const Camera& orig);
    virtual ~Camera();
    
    void envoyerPhoto();
    void envoyerMire();
    void enregistrerPhoto();

private:
    unsigned long frequence;
    const std::string indicatif;
    int i;

};



#endif /* CAM_H */

