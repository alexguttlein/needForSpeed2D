#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "common/eventType.h"
#include "carStateDTO.h"
#include <vector>
#include "gameInfo.h"

#pragma pack(push, 1)  // para evitar padding
struct Snapshot {
    EventType controlEvent;
    int playerId;
    uint32_t playersSize;
    std::vector<CarStateDTO> cars;
    std::vector<GameInfo> gameList;
    Snapshot () = default;
};
#pragma pack(pop)

#endif //SNAPSHOT_H
