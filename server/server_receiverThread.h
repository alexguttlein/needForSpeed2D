#ifndef SERVER_RECEIVERTHREAD_H
#define SERVER_RECEIVERTHREAD_H

#include "../common/thread.h"
#include "../common/queue.h"
#include "server/server_protocol.h"

class ReceiverThread : public Thread {
public:
    ReceiverThread(ServerProtocol& protocol, Queue<std::shared_ptr<uint8_t>>& client_queue);
    virtual void run() override;

private:
    ServerProtocol& protocol;
    Queue<std::shared_ptr<uint8_t>>& client_queue;
};

#endif //SERVER_RECEIVERTHREAD_H