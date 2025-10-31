#include "server_game.h"

Game::Game(int gameId)
    : gameId(gameId), sharedQueue(Constants::GAME_QUEUE_MAXSIZE) {}

Queue<std::shared_ptr<Message>>& Game::getSharedQueue() {
    return sharedQueue;
}
