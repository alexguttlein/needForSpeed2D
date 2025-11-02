#pragma once

#include <cstdint>
#include <vector>
#include "vector2D.h"
#include "carStateDTO.h"


// snapshot del juego
struct GameSnapshot {
    uint8_t num_cars; // Número de autos activos en esta instantánea (0 a MAX_CARS)
    CarStateDTO car_states[8]; 
};

