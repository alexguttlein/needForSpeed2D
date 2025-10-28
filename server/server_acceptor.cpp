#include "server_acceptor.h"

Acceptor::Acceptor(const char* port, MonitorClients& monitorClients) :
    socket(port),
    monitorClients(monitorClients),
    keepAccepting(true) {}

void Acceptor::run() {

    while (keepAccepting && !socket.is_stream_recv_closed()) {
        try {
            // se acepta un nuevo cliente
            Socket newSocket = socket.accept();
            int id = newSocket.get_fd();
            
            // Prueba de comunicación simple
            // uint8_t msg = 0x00;
            // newSocket.sendall(&msg, sizeof(msg));
            // bool seguir = true;
            // while (seguir) {
            //     uint8_t msg2;
            //     newSocket.recvall(&msg2, sizeof(msg2));
            //     if (msg2 == 0x01) {
            //         std::cout << "comando recibido" << std::endl;
            //         uint8_t msg3 = 0x10;
            //         newSocket.sendall(&msg3, sizeof(msg3));
            //     } else if (msg2 == 0x02) {
            //         std::cout << "comando recibido" << std::endl;
            //         uint8_t msg3 = 0x11;
            //         newSocket.sendall(&msg3, sizeof(msg3));
            //     } else if (msg2 == 0x03) {
            //         std::cout << "comando recibido" << std::endl;
            //         uint8_t msg3 = 0x12;
            //         newSocket.sendall(&msg3, sizeof(msg3));
            //     } else if (msg2 == 0x04) {
            //         std::cout << "comando recibido" << std::endl;
            //         uint8_t msg3 = 0x13;
            //         newSocket.sendall(&msg3, sizeof(msg3));
            //     }
            //     else{
            //         seguir = false;
            //     }
            // }
            
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
