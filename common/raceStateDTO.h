#ifndef RACE_STATE_DTO_H
#define RACE_STATE_DTO_H

#include "vector2D.h"
#include <vector>

/*
* Data Transfer Object (DTO) para el estado de la carrera
*
* */
struct RaceStateDTO {
    Vector2D<float> nextCheckpoint;
    std::vector<Vector2D<float>> currentHints;
    bool hasFinished;
    int finishPosition;    
};
#endif // RACE_STATE_DTO_H
