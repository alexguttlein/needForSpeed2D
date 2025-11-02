#ifndef SERVER_GAMEMONITOR_H
#define SERVER_GAMEMONITOR_H

#include <server/server_game.h>
#include "common/queue.h"
#include "common/message.h"
#include <map>
#include <mutex>
#include <memory>

class GameMonitor {
private:
    std::mutex mtx;
    std::map<int, std::unique_ptr<Game>> games;
    int gameId = 0;
public:
    GameMonitor();
    int createGame();
    Queue<std::shared_ptr<Message>>& getGameQueue(int gameId);
    bool tryJoinGame(int id, Queue<std::shared_ptr<Message>>*& outQueue);
    void leaveGame(int id);
};

#endif //SERVER_GAMEMONITOR_H