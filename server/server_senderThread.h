#ifndef SERVER_SENDERTHREAD_H
#define SERVER_SENDERTHREAD_H

#include "../common/thread.h"
#include "../common/queue.h"
#include "common/message.h"
#include "common/constants.h"
#include "common/snapshot.h"
#include "server/server_protocol.h"
#include "common/eventType.h"

class SenderThread : public Thread {
public:
    SenderThread(ServerProtocol& protocol, Queue<Snapshot>& clientQueue);
    virtual void run() override;

private:
    ServerProtocol& protocol;
    bool keepRunning;
    Queue<Snapshot>& clientQueue;
};

#endif //SERVER_SENDERTHREAD_H