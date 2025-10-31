#include "server_gameMonitor.h"

GameMonitor::GameMonitor() : games() {}

int GameMonitor::createGame() {
    std::lock_guard<std::mutex> lock(mtx);
    gameId++;
    games[gameId] = std::make_unique<Game>(gameId);
    return gameId;
}

Queue<std::shared_ptr<Message>>& GameMonitor::getGameQueue(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    return games.at(id)->getSharedQueue();
}
