#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "common/socket.h"
#include "common/queue.h"
#include "common/constants.h"
#include "server/server_protocol.h"
#include "server/server_senderThread.h"
#include "server/server_receiverThread.h"
#include <algorithm>

class ClientHandler {
public:
    explicit ClientHandler(Socket socket);

private:
    ServerProtocol protocol;
    Queue<std::shared_ptr<uint8_t>> clientQueue;
    SenderThread senderThread;
    ReceiverThread receiverThread;
};

#endif //CLIENTHANDLER_H