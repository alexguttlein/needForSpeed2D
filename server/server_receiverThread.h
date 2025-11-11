#ifndef SERVER_RECEIVERTHREAD_H
#define SERVER_RECEIVERTHREAD_H

#include "../common/thread.h"
#include "../common/queue.h"
#include "common/message.h"
#include "server/server_protocol.h"
#include "server/server_gameMonitor.h"

class GameMonitor;
class ClientHandler;
class ReceiverThread : public Thread {
public:
    ReceiverThread(ServerProtocol& protocol, GameMonitor& gameMonitor, ClientHandler& handler);
    void lobbyCommands(Message msg);
    virtual void run() override;
    void stop() override;
    ~ReceiverThread() override;

private:
    ServerProtocol& protocol;
    GameMonitor& gameMonitor;
    ClientHandler& clientHandler;
    Queue<std::shared_ptr<Message>>* gameQueue;
    bool keepRunning;
};

#endif //SERVER_RECEIVERTHREAD_H