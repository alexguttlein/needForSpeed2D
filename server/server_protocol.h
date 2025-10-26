#ifndef TALLER_TP_SERVER_PROTOCOL_H
#define TALLER_TP_SERVER_PROTOCOL_H

#include "common/socket.h"
#include <arpa/inet.h>

class ServerProtocol {
public:
    explicit ServerProtocol(Socket socket);
private:
    Socket socket;
    bool isClosed;
    void closeSocket();
};

#endif //TALLER_TP_SERVER_PROTOCOL_H