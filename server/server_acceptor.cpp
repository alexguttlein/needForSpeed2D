#include "server_acceptor.h"

Acceptor::Acceptor(const char* port) :
    socket(port),
    keepAccepting(true),
    clients(), gameMonitor() {}

void Acceptor::run() {
    while (keepAccepting) {
        try {
            Socket newSocket = socket.accept();
            int id = newSocket.get_fd();

            ClientHandler* client = new ClientHandler(std::move(newSocket), id, this->gameMonitor);

            client->startThreads();

            clients.push_back(client);
            killDeadClients();
        } catch (const std::exception& e) {
            if (keepAccepting) {
                std::cerr << "Unexpected exception: " << e.what() << std::endl;
            }
            break;
        }
    }
    if (!socket.is_stream_recv_closed()) {
        socket.shutdown(SHUT_RDWR);
    }
}

void Acceptor::endAccepting() {
    keepAccepting = false;
    closeSocket();
    killDeadClients();
}

void Acceptor::closeSocket() {
    if (!socket.is_stream_recv_closed() && !socket.is_stream_send_closed()) {
        this->socket.shutdown(SHUT_RDWR);
    }
}

void Acceptor::killDeadClients() {
    clients.remove_if([this](ClientHandler* client) {
        if (!client->isAlive()) {
            killClient(client);
            return true;
        }
        return false;
    });
}

void Acceptor::killClient(ClientHandler* client) {
    if (!client) return;
    try { client->shutdown(); } catch(...) {}  // asegura que Game ya no apunte a su queue
    delete client;
}

Acceptor::~Acceptor() {
    for (ClientHandler* client : clients) {
        if (client) {
            try { client->shutdown(); } catch(...) {}
            delete client;
            client = nullptr;
        }
    }
    clients.clear();

    if (!socket.is_stream_send_closed() || !socket.is_stream_recv_closed()) {
        socket.shutdown(SHUT_RDWR);
        socket.close();
    }
}
