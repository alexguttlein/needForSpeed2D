#include "snapshots.h"


void Snapshots::addSnapshot(const Snapshot& snapshot) {
    std::unique_lock<std::mutex> lock(mtx);
    snapshot_queue.try_push(snapshot);
}


bool Snapshots::popSnapshot(Snapshot& snapshot) {
    std::unique_lock<std::mutex> lock(mtx);
    if (snapshot_queue.try_pop(snapshot)) {
        return true;
    }

    return false;
}

void Snapshots::close() { snapshot_queue.close(); }
