#include "server_senderThread.h"

SenderThread::SenderThread(ServerProtocol& protocol, Queue<Snapshot>& clientQueue) :
    protocol(protocol), keepRunning(true), clientQueue(clientQueue) {
}

void SenderThread::run() {
    std::cout << "SenderThread::run()" << std::endl;
    while (keepRunning) {
        Snapshot snapshot = clientQueue.pop();

        if (protocol.isConnectionClosed()) {
            keepRunning = false;
            break;
        }
        // std::cout << "en sender se hizo pop() " << snapshot.posY << std::endl;
        protocol.sendMessage(snapshot);
    }
}
