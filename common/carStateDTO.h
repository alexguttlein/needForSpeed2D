#ifndef CARSTATEDTO_H
#define CARSTATEDTO_H

#include <cstdint>
#include "vector2D.h"


/*
* Data Transfer Object (DTO) para el estado del auto
*
* */
struct CarStateDTO {
    int car_id;       
    float health;       
    Vector2D<float> position; 
    Vector2D<float> angle;
    // float speed;
};
#endif // CARSTATEDTO_H
