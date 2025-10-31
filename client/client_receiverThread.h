#ifndef CLIENT_RECEIVERTHREAD_H
#define CLIENT_RECEIVERTHREAD_H

#include "../common/thread.h"
#include "../common/queue.h"
#include "client/client_protocol.h"

#include <iostream>

class ReceiverThread : public Thread {
public:
    ReceiverThread(ClientProtocol& protocol, Queue<Snapshot>& queue);
    void run() override;
private:
    ClientProtocol& protocol;
    Queue<Snapshot>& snapshotQueue;
    bool keepRunning;
};

#endif //CLIENT_RECEIVERTHREAD_H
