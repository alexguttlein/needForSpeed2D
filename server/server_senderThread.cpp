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
            // protocol.sendControl(Constants::CREATE_JOIN_ACCEPTED);
            // Enviar CREATE_JOIN_ACCEPTED + playerId
            if (protocol.isConnectionClosed()) continue;

            std::vector<uint8_t> buffer;
            buffer.push_back(Constants::TYPE_CONTROL);
            buffer.push_back(Constants::CREATE_JOIN_ACCEPTED);
            protocol.addIntToUint8tVector(buffer, snapshot->playerId);
            protocol.sendCreateJoinAccepted(buffer);
        } else if (snapshot->controlEvent == EventType::GAME_START) {
            if (protocol.isConnectionClosed()) continue;
            std::vector<uint8_t> buffer;
            buffer.push_back(Constants::TYPE_CONTROL);
            buffer.push_back(Constants::GAME_START);
            protocol.sendGameStart(buffer);
        } else {
            // std::cout << "[Sender] aca se deberia enviar snap" << std::endl;
            // std::cout << "debug: el snap va a ser: " << snapshot->posX << ", " << snapshot->posY << std::endl;
            protocol.sendSnapshot(snapshot);
        }

        // iter++;
        // if (iter % 10 == 0) {
        //     std::cout << "[Sender] iteraciones totales: " << iter << std::endl;
        // }

        if (protocol.isConnectionClosed()) {
            keepRunning = false;
            break;
        }

        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void SenderThread::stop() {
    keepRunning = false;
    protocol.closeSocket();
}
