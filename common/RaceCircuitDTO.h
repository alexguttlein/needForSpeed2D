#ifndef RACE_CIRCUIT_DTO_H
#define RACE_CIRCUIT_DTO_H

#include "vector2D.h"
#include <vector>
#include <string>

/*
* Estructura que representa un circuito de carrera
* Usado para cargar los checkpoints desde YAML y identificar el circuito
* */
struct RaceCircuit {
    std::string race_id;
    std::vector<Vector2D<float>> checkpoints;
};
#endif // RACE_CIRCUIT_DTO_H