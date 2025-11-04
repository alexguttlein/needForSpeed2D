#ifndef CLIENT_RECEIVERTHREAD_H
#define CLIENT_RECEIVERTHREAD_H

#include "../common/thread.h"
#include "../common/queue.h"
#include "client_protocol.h"
#include "../common/eventType.h"

#include <iostream>

class ReceiverThread : public Thread {
public:
    ReceiverThread(ClientProtocol& protocol, Queue<Snapshot>& queue, Queue<Event>& eventQueue);
    void run() override;
private:
    ClientProtocol& protocol;
    Queue<Snapshot>& snapshotQueue;
    Queue<Event>& eventQueue;
    bool keepRunning;
};

#endif //CLIENT_RECEIVERTHREAD_H
