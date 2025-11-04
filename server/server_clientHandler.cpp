#include "server_clientHandler.h"
#include "server_gameMonitor.h"
#include "server_receiverThread.h"
#include <iostream>

ClientHandler::ClientHandler(Socket socket, int id, GameMonitor& gameMonitor) :
    protocol(std::move(socket)), gameMonitor(gameMonitor),
        clientQueue(Constants::CLIENT_QUEUE_MAXSIZE),
        sharedQueue(nullptr),
        senderThread(protocol, clientQueue),
        // receiverThread(protocol, gameMonitor, *this),
        receiverThread(std::make_unique<ReceiverThread>(protocol, gameMonitor, *this)),
        id(id), alive(true), snapshot(), currentGameId(0) {}

void ClientHandler::startThreads() {
    std::cout << "DEBUG: Client threads starting..." << std::endl;
    receiverThread->start();
    senderThread.start();
}

void ClientHandler::shutdown() {
    try {
        clientQueue.close();

        senderThread.join();
        receiverThread->join();

        if (!protocol.isConnectionClosed()) {
            protocol.closeSocket();
        }

        if (currentGameId == 0) gameMonitor.leaveGame(currentGameId);
    } catch (const std::exception& e) {
        std::cerr << "ClientHandler::shutdown exception: " << e.what() << std::endl;
    }
}

bool ClientHandler::isConnected() const {
    return !protocol.isConnectionClosed();
}

bool ClientHandler::isAlive() const {
    return alive;
}

void ClientHandler::killClient() {
    alive = false;
}

void ClientHandler::assignGameQueue(Queue<std::shared_ptr<Message>>& queue, int gameId) {
    sharedQueue = &queue;
    currentGameId = gameId;
}

int ClientHandler::getId() const { return id; }

int ClientHandler::getCurrentGameId() const { return currentGameId; }

Queue<std::shared_ptr<Snapshot>>& ClientHandler::getClientQueue() {
    return clientQueue;
}

ClientHandler::~ClientHandler() = default;
