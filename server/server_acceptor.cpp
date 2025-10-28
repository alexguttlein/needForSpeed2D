#include "server_acceptor.h"

Acceptor::Acceptor(const char* port, MonitorClients& monitorClients) :
    socket(port),
    monitorClients(monitorClients),
    keepAccepting(true) {}

void Acceptor::run() {

    while (keepAccepting && !socket.is_stream_recv_closed()) {
        try {
            Socket newSocket = socket.accept();
            int id = newSocket.get_fd();
            
            addNewClient(id, std::move(newSocket));

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

void Acceptor::addNewClient(int id, Socket newSocket) {
    ClientHandler& client = monitorClients.insertClient(id, std::move(newSocket));
    std::cout << "Cliente agregado con id: " << id << std::endl;
    client.startThreads();
}
