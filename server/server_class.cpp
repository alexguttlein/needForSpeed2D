#include "server_class.h"

Server::Server(const char* port) : acceptor(port) {
    acceptor.start();
}

void Server::run() {
    std::string end;
    while (std::getline(std::cin, end)) {
        if (end == "q") {
            closeAcceptor();
            break;
        }
    }
}

void Server::closeAcceptor() {
    acceptor.endAccepting();
    acceptor.join();
}

Server::~Server() {
    closeAcceptor();
}
