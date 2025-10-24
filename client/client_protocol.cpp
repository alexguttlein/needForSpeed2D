#include "client_protocol.h"

ClientProtocol::ClientProtocol(const char* host, const char* port) :
    socket(host, port),
    isClosed(false) {}

ClientProtocol::~ClientProtocol() {
    if (!isClosed && !socket.is_stream_recv_closed()) {
        socket.shutdown(SHUT_RDWR);
        socket.close();
        isClosed = true;
    }
}

void ClientProtocol::send(uint8_t& msg) {
    socket.sendall(&msg, sizeof(msg));
}

uint8_t ClientProtocol::recibir() {
    uint8_t msg;
    socket.recvall(&msg, sizeof(msg));
    return msg;
}
