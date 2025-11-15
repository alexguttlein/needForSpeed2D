#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "common/eventType.h"
#include "carStateDTO.h"
#include "raceStateDTO.h"
#include <vector>

#pragma pack(push, 1)  // para evitar padding
struct Snapshot {
    EventType controlEvent;
    int playerId;
    uint32_t playersSize;
    std::vector<CarStateDTO> cars;
    RaceStateDTO raceState;
    Snapshot () = default;
};
#pragma pack(pop)

#endif //SNAPSHOT_H
