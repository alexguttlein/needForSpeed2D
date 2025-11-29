#ifndef RACE_STATE_DTO_H
#define RACE_STATE_DTO_H

#include "vector2D.h"
#include <vector>
#include <string>

/*
* Data Transfer Object (DTO) para el estado de la carrera
*
* */
struct RaceStateDTO {
    std::string playerName;
    int playerId;
    std::string timeLeftRace; // Formato "MM:SS"
    int currentRaceId;
    Vector2D<float> nextCheckpoint;
    std::vector<Vector2D<float>> currentHints;
    bool hasFinished;
    int finishPosition;
    float finishTimeSeconds;   
};
#endif // RACE_STATE_DTO_H
