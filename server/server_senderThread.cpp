#include "server_senderThread.h"

SenderThread::SenderThread(ServerProtocol& protocol, Queue<std::shared_ptr<Snapshot>>& clientQueue) :
    protocol(protocol), keepRunning(true), clientQueue(clientQueue) {
}

void SenderThread::run() {

    while (keepRunning) {
        std::shared_ptr<Snapshot> snapshot;
        try {
            snapshot = clientQueue.pop();
        } catch (const ClosedQueue&) {
            break;
        }

        if (!snapshot) {
            continue;
        }

        if (snapshot->controlEvent == EventType::JOIN_REJECTED) {
            protocol.sendControl(Constants::JOIN_REJECTED);
        } else if (snapshot->controlEvent == EventType::CREATE_JOIN_ACCEPTED) {
            // Enviar CREATE_JOIN_ACCEPTED + playerId
            if (protocol.isConnectionClosed()) continue;

            std::vector<uint8_t> buffer;
            buffer.push_back(Constants::TYPE_CONTROL);
            buffer.push_back(Constants::CREATE_JOIN_ACCEPTED);
            protocol.addIntToUint8tVector(buffer, snapshot->playerId);
            // protocol.sendCreateJoinAccepted(buffer);
            protocol.sendControl(buffer);
        } else if (snapshot->controlEvent == EventType::GAME_START) {
            if (protocol.isConnectionClosed()) continue;
            std::vector<uint8_t> buffer;
            buffer.push_back(Constants::TYPE_CONTROL);
            buffer.push_back(Constants::GAME_START);
            // protocol.sendGameStart(buffer);
            protocol.sendControl(buffer);
        } else if (snapshot->controlEvent == EventType::SERVER_DISCONNECTED) {
            if (protocol.isConnectionClosed()) continue;
            std::cout << "debug: Disconnected" << std::endl;
            std::vector<uint8_t> buffer;
            buffer.push_back(Constants::TYPE_CONTROL);
            buffer.push_back(Constants::SERVER_DISCONNECTED);
            protocol.sendControl(buffer);
        } else {
            protocol.sendSnapshot(snapshot);
        }

        if (protocol.isConnectionClosed()) {
            keepRunning = false;
            break;
        }
    }
}

void SenderThread::stop() {
    keepRunning = false;
    protocol.closeSocket();
}
