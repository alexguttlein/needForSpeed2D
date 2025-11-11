#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "common/queue.h"
#include "common/message.h"
#include "common/constants.h"
#include "server/server_clientHandler.h"
#include <vector>
#include <memory>
#include <mutex>

class ClientHandler;
class GameLoop;

class Game {
public:
    explicit Game(int gameId);
    Queue<std::shared_ptr<Message>>& getSharedQueue();
    int totalPlayers;
    void addClientHandler(ClientHandler* client);
    void removeClientHandler(ClientHandler* client);
    ~Game();
private:
    int gameId;
    Queue<std::shared_ptr<Message>> sharedQueue;
    std::vector<Queue<std::shared_ptr<Snapshot>>*> clientQueues; // lista de client queues para broadcast
    std::vector<ClientHandler*> clientHandlers;
    std::mutex mtx;
    std::unique_ptr<GameLoop> gameloop;
    void closeAllClientQueues();
};
#endif //SERVER_GAME_H