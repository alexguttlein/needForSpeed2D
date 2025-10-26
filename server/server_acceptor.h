#ifndef SERVER_ACCEPTOR_H
#define SERVER_ACCEPTOR_H

#include "../common/socket.h"
#include "../common/thread.h"
#include "server_clientHandler.h"

#include <arpa/inet.h>

class Acceptor : public Thread {
public:
    Acceptor(const char* port);
    void run() override;
    void endAccepting();
    // ~Acceptor();

private:
    Socket socket;
    bool keepAccepting;
    void closeSocket();
};

#endif //SERVER_ACCEPTOR_H
