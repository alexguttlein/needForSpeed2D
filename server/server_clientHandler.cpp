#include "server_clientHandler.h"

ClientHandler::ClientHandler(Socket socket) :
    protocol(std::move(socket)),
        clientQueue(Constants::CLIENT_QUEUE_MAXSIZE),
        senderThread(protocol, clientQueue),
        receiverThread(protocol, clientQueue) {
    senderThread.start();
    receiverThread.start();
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