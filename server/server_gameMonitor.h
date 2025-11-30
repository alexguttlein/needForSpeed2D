#ifndef SERVER_GAMEMONITOR_H
#define SERVER_GAMEMONITOR_H

#include <server/server_game.h>
#include "common/queue.h"
#include "common/message.h"
#include <map>
#include <mutex>
#include <memory>

class ClientHandler;

class GameMonitor {
private:
    std::mutex mtx;
    std::map<int, std::unique_ptr<Game>> games;
    int gameId = 0;
public:
    GameMonitor();
    ~GameMonitor();
    int createGame(std::string& gameCreator);
    Queue<std::shared_ptr<Message>>& getGameQueue(int gameId);
    bool tryJoinGame(int id, Queue<std::shared_ptr<Message>>*& outQueue);
    bool registerClientToGame(int id, ClientHandler* client);
    void leaveGame(int id);
    std::vector<std::pair<int,int>> listGames();
    void unregisterClientFromGame(int id, ClientHandler* client);
    std::string getGameCreator(int gameId);
    void checkGameStart(int gameId);
    void startGame(int matchId);
};

#endif //SERVER_GAMEMONITOR_H