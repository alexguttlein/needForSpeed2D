#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "../common/constants.h"
#include "../common/queue.h"
#include "client_protocol.h"
#include "client_receiverThread.h"

class Client {
public:
    Client(const char* host, const char* port);
    void run();
private:
    ClientProtocol protocol;
    Queue<Snapshot> snapshotQueue;
    ReceiverThread receiver;
};

#endif //CLIENT_H
