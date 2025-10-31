#include "server_acceptor.h"

Acceptor::Acceptor(const char* port) :
    socket(port),
    keepAccepting(true),
    clients() {}

void Acceptor::run() {
    GameMonitor gameMonitor;
    while (keepAccepting && !socket.is_stream_recv_closed()) {
        try {
            Socket newSocket = socket.accept();
            int id = newSocket.get_fd();

            ClientHandler* client = new ClientHandler(std::move(newSocket), id, gameMonitor);

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
}

void Acceptor::endAccepting() {
    keepAccepting = false;
    closeSocket();
}

void Acceptor::closeSocket() {
    if (!socket.is_stream_recv_closed() && !socket.is_stream_send_closed()) {
        this->socket.shutdown(SHUT_RDWR);
        this->socket.close();
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
    client->killClient();
    delete client;
}

Acceptor::~Acceptor() {
    for (auto &client : clients) {
        killClient(client);
    }
    clients.clear();
    socket.shutdown(SHUT_RDWR);
    socket.close();
}
