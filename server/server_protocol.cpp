#include "server_protocol.h"

#include <algorithm>

ServerProtocol::ServerProtocol(Socket socket) :
    socket(std::move(socket)) , isClosed(false) {}

void ServerProtocol::closeSocket() {
    if (isClosed) return;
    socket.shutdown(SHUT_RDWR);
    socket.close();
}

bool ServerProtocol::isConnectionClosed() const {
    return socket.is_stream_send_closed() || socket.is_stream_recv_closed();
}
