#include "server_senderThread.h"

SenderThread::SenderThread(ServerProtocol& protocol, Queue<Snapshot>& clientQueue) :
    protocol(protocol), keepRunning(true), clientQueue(clientQueue) {
}

void SenderThread::run() {
    std::cout << "debug: SenderThread::run()" << std::endl;
    while (keepRunning) {
        Snapshot snapshot = clientQueue.pop();

        if (snapshot.controlEvent == EventType::JOIN_REJECTED) {
            protocol.sendControl(Constants::JOIN_REJECTED);
        } else if (snapshot.controlEvent == EventType::CREATE_JOIN_ACCEPTED) {
            protocol.sendControl(Constants::CREATE_JOIN_ACCEPTED);
        } else {
            protocol.sendSnapshot(snapshot);
        }

        if (protocol.isConnectionClosed()) {
            keepRunning = false;
            break;
        }
        // std::cout << "en sender se hizo pop() " << snapshot.posY << std::endl;
        // protocol.sendSnapshot(snapshot);
    }
}
