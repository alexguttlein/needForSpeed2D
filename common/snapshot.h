#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "common/eventType.h"
#include "common/player.h"
#include <vector>

#pragma pack(push, 1)  // para evitar padding
struct Snapshot {
    EventType controlEvent;
    int playerId;
    uint32_t playersSize;
    std::vector<Player> players;
    // uint32_t posX;
    // uint32_t posY;
    Snapshot () = default;
};
#pragma pack(pop)

#endif //SNAPSHOT_H
