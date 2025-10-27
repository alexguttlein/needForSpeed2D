#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "common/socket.h"
#include "common/queue.h"
#include "common/constants.h"
#include "common/message.h"
#include "server/server_protocol.h"
#include "server/server_senderThread.h"
#include "server/server_receiverThread.h"
#include "server/server_snapshots.h"

#include <algorithm>

class ClientHandler {
public:
    explicit ClientHandler(Socket socket);
    void enqueueMessage(const std::shared_ptr<Message>& msg);
    bool isConnected() const;
    Snapshots& getSnapshots();

private:
    ServerProtocol protocol;
    Queue<std::shared_ptr<Message>> clientQueue;
    Snapshots snapshots;
    SenderThread senderThread;
    ReceiverThread receiverThread;
};

#endif //CLIENTHANDLER_H