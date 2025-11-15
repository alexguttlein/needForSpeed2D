#include "server_clientHandler.h"
#include "server_gameMonitor.h"
#include "server_receiverThread.h"
#include <iostream>

ClientHandler::ClientHandler(Socket socket, int id, GameMonitor& gameMonitor) :
    protocol(std::move(socket)), gameMonitor(gameMonitor),
        clientQueue(Constants::CLIENT_QUEUE_MAXSIZE),
        sharedQueue(nullptr),
        senderThread(protocol, clientQueue),
        receiverThread(std::make_unique<ReceiverThread>(protocol, gameMonitor, *this)),
        id(id), alive(true), snapshot(), currentGameId(0) {}

void ClientHandler::startThreads() {
    receiverThread->start();
    senderThread.start();
}

void ClientHandler::shutdown() {
    std::cout << "debug: ClientHandler::shutdown()" << std::endl;
    // se evita que shutdown ejecute más de una vez
    bool expected = false;
    if (!shuttingDown.compare_exchange_strong(expected, true)) {
        return;
    }

    alive = false;

    // se cierra la queue del cliente
    try { clientQueue.close(); } catch(...) {}

    // if (currentGameId != 0) {
    //     gameMonitor.leaveGame(currentGameId);
    // }

    // se termina el sender y receiver
    try { senderThread.stop(); } catch(...) {}
    try { senderThread.join(); } catch(...) {}
    if (receiverThread) {
        try { receiverThread->stop(); } catch(...) {}
        try { receiverThread->join(); } catch(...) {}
    }

    // se cierra el socket si sigue abierto
    try {
        if (!protocol.isConnectionClosed()) {
            protocol.closeSocket();
        }
    } catch (...) {}

    if (currentGameId != 0) {
        // notifica y remueve este ClientHandler de la Game correspondiente,
        // para que Game elimine su puntero a la queue *antes* de que este object sea destruido.
        gameMonitor.unregisterClientFromGame(currentGameId, this);
        currentGameId = 0;
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
    // shutdown();
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

ClientHandler::~ClientHandler() {
    std::cout << "debug: destruyendo clientHandler" << std::endl;
    // try { shutdown(); } catch(...) {}
};
