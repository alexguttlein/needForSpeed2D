#include "server_clientHandler.h"

ClientHandler::ClientHandler(Socket socket) :
    protocol(std::move(socket)),
        clientQueue(Constants::CLIENT_QUEUE_MAXSIZE),
        senderThread(protocol, clientQueue),
        receiverThread(protocol, clientQueue) {
    senderThread.start();
    receiverThread.start();
}
