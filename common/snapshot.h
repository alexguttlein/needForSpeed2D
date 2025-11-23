#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "common/eventType.h"
#include "carStateDTO.h"
#include "raceStateDTO.h"
#include "../server/leaderBoard.h"
#include <vector>
#include "gameInfo.h"

#pragma pack(push, 1)  // para evitar padding
struct Snapshot {
    EventType controlEvent;
    int playerId;
    uint32_t playersSize;
    std::vector<CarStateDTO> cars;
    std::vector<GameInfo> gameList;
    std::vector<RaceStateDTO> raceStates;
    bool raceFinished = false;      // true cuando todos los jugadores terminaron
    bool gameFinished = false;  // true cuando no hay más circuitos
    std::vector<PlayerTime> leaderboards; // Leaderboard final partida
    Snapshot () = default;
};
#pragma pack(pop)

#endif //SNAPSHOT_H
