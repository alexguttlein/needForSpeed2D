#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "common/eventType.h"

#pragma pack(push, 1)  // para evitar padding
struct Snapshot {
    EventType controlEvent;
    uint32_t posX;
    uint32_t posY;
    Snapshot () = default;
};
#pragma pack(pop)

#endif //SNAPSHOT_H
