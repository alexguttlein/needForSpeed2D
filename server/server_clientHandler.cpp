#include "server_clientHandler.h"

ClientHandler::ClientHandler(Socket socket) :
    protocol(std::move(socket)),
        clientQueue(Constants::CLIENT_QUEUE_MAXSIZE),
        senderThread(protocol, clientQueue),
        receiverThread(protocol, clientQueue) {
}


void ClientHandler::startThreads() {
    try {
        uint8_t init = 0x00;
        protocol.getSocket().sendall(&init, sizeof(init));

        // loop de handshake/comandos corto; sale al recibir algo distinto
        while (!protocol.isConnectionClosed()) {
            uint8_t cmd;
            protocol.getSocket().recvall(&cmd, sizeof(cmd));

            if (cmd == 0x01) {
                uint8_t resp = 0x10;
                protocol.getSocket().sendall(&resp, sizeof(resp));
                std::cout << "comando recibido: w" << std::endl;
            } else if (cmd == 0x02) {
                uint8_t resp = 0x11;
                protocol.getSocket().sendall(&resp, sizeof(resp));
                std::cout << "comando recibido: s" << std::endl;
            } else if (cmd == 0x03) {
                uint8_t resp = 0x12;
                protocol.getSocket().sendall(&resp, sizeof(resp));
                std::cout << "comando recibido: a" << std::endl;
            } else if (cmd == 0x04) {
                uint8_t resp = 0x13;
                protocol.getSocket().sendall(&resp, sizeof(resp));
                std::cout << "comando recibido: d" << std::endl;
            } else {
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "ClientHandler handshake error: " << e.what() << std::endl;
    }
    senderThread.start();
    receiverThread.start();
}



void ClientHandler::shutdown() {
    try {
        clientQueue.close();

        senderThread.join();
        receiverThread.join();

        if (!protocol.isConnectionClosed()) {
            protocol.closeSocket();
        }
    } catch (const std::exception& e) {
        std::cerr << "ClientHandler::shutdown exception: " << e.what() << std::endl;
    }
}


void ClientHandler::enqueueMessage(const std::shared_ptr<Message>& msg) {
    clientQueue.try_push(msg);
}

bool ClientHandler::isConnected() const {
    return !protocol.isConnectionClosed();
}


Snapshots& ClientHandler::getSnapshots() {
    return snapshots;
}
