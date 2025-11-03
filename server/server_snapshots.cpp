#include "server_snapshots.h"


void WorldSnapshots::addSnapshot(const GameSnapshot& snapshot) {
    snapshot_queue.push(snapshot);
}


bool WorldSnapshots::popSnapshot(GameSnapshot& snapshot) {
    return snapshot_queue.try_pop(snapshot);
}

void WorldSnapshots::close() { snapshot_queue.close(); }


WorldSnapshots::~WorldSnapshots() {
    close();
}
