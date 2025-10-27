#include "server_class.h"

Server::Server(const char* port) : commandQueue(), monitor(), acceptor(port), gameLoop(commandQueue, monitor) {

    acceptor.start();
    gameLoop.start();
}

void Server::run() {
    std::string end;
    while (std::getline(std::cin, end)) {
        if (end == "q") {
            //closeAcceptor();
            break;
        }
    }
}

void Server::closeAcceptor() {
    acceptor.endAccepting();
    acceptor.join();
}

void Server::closeGameLoop() {
    gameLoop.stop();
    gameLoop.join();
}

void Server::closeClients() {
    monitor.clear();
}


Server::~Server() {
    closeGameLoop();
    closeAcceptor();
    closeClients();
}
