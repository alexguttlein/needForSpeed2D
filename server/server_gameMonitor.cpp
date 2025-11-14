#include "server_gameMonitor.h"
#include "server/server_clientHandler.h"

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

bool GameMonitor::tryJoinGame(int id, Queue<std::shared_ptr<Message>>*& outQueue) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = games.find(id);
    if (it == games.end()) return false; //la partida no existe

    Game* game = it->second.get();
    if (game->totalPlayers >= Constants::MAX_PLAYERS_IN_GAME) {
        return false; // partida completa, no se puede unir
    }

    outQueue = &game->getSharedQueue();
    return true;
}

void GameMonitor::leaveGame(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = games.find(id);
    if (it == games.end()) return;
    Game* game = it->second.get();
    if (game->totalPlayers > 0) game->totalPlayers--;
    // si totalPlayers == 0 se borra la partida
    if (game->totalPlayers == 0) games.erase(it);
}

std::vector<std::pair<int, int>> GameMonitor::listGames() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::pair<int,int>> result;
    for (auto& [id, gamePtr] : games) {
        result.emplace_back(id, gamePtr->totalPlayers);
    }
    return result;
}

bool GameMonitor::registerClientToGame(int id, ClientHandler* client) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = games.find(id);
    if (it == games.end()) return false;
    Game* game = it->second.get();

    //se agrega cliente al juego y se aumenta el numero de jugadores
    game->addClientHandler(client);
    game->totalPlayers++;
    return true;
}

GameMonitor::~GameMonitor() {
    std::lock_guard<std::mutex> lock(mtx);
    games.clear();
}
