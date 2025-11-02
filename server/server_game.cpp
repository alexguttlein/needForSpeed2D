#include "server_game.h"

#include <iostream>

Game::Game(int gameId) :
    totalPlayers(0),
    gameId(gameId),
    sharedQueue(Constants::GAME_QUEUE_MAXSIZE) {}

Queue<std::shared_ptr<Message>>& Game::getSharedQueue() {
    std::cout << "debug: se recupera la queue de la partida " << gameId << std::endl;
    return sharedQueue;
}
