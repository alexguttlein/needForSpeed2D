#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "common/eventType.h"

struct Snapshot {
    EventType controlEvent;
    int posX;
    int posY;
    Snapshot () = default;
};

#endif //SNAPSHOT_H
