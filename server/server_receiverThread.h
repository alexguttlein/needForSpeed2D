#ifndef SERVER_RECEIVERTHREAD_H
#define SERVER_RECEIVERTHREAD_H

#include "../common/thread.h"
#include "../common/queue.h"
#include "common/message.h"
#include "server/server_protocol.h"
#include "server/server_gameMonitor.h"

class ClientHandler;
class ReceiverThread : public Thread {
public:
    // ReceiverThread(ServerProtocol& protocol, Queue<std::shared_ptr<Message>>& serverQueue);
    ReceiverThread(ServerProtocol& protocol, GameMonitor& gameMonitor, ClientHandler& handler);
    virtual void run() override;

private:
    ServerProtocol& protocol;
    GameMonitor& gameMonitor;
    ClientHandler& clientHandler;
    Queue<std::shared_ptr<Message>>* gameQueue;
    bool keepRunning;
};

#endif //SERVER_RECEIVERTHREAD_H