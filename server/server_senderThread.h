#ifndef SERVER_SENDERTHREAD_H
#define SERVER_SENDERTHREAD_H

#include "../common/thread.h"
#include "../common/queue.h"
#include "common/message.h"
#include "server/server_protocol.h"

class SenderThread : public Thread {
public:
    SenderThread(ServerProtocol& protocol, Queue<std::shared_ptr<Message>>& client_queue);
    virtual void run() override;

private:
    ServerProtocol& protocol;
    Queue<std::shared_ptr<Message>>& client_queue;
};

#endif //SERVER_SENDERTHREAD_H