#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <vector>

/*
* Estructura que representa el tiempo de finalización de una partida de un jugador
* Con esto vamos a crear el leaderboard final
*/
struct PlayerTime {
    int playerId;
    float finishTime;
};

using Leaderboard = std::vector<PlayerTime>;

#endif // LEADERBOARD_H