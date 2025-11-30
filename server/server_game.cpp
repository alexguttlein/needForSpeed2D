#include "server_game.h"
#include "server/server_gameloop.h"
#include <iostream>

Game::Game(int gameId, std::string& gameCreator) :
    totalPlayers(0),
    gameId(gameId),
    sharedQueue(Constants::GAME_QUEUE_MAXSIZE),
    clientQueues(),
    clientHandlers(),
    gameloop(nullptr),
    gameCreator(gameCreator),
    gameStarted(false) {}

Queue<std::shared_ptr<Message>>& Game::getSharedQueue() {
    return sharedQueue;
}

void Game::checkGameStart() {

    if (((int)clientQueues.size() >= Constants::MAX_PLAYERS_IN_GAME || gameStarted)
            && !gameloop && (int)clientQueues.size() > 1) {

        //se avisa a todos los usuarios que el juego va a comenzar
        for (auto* q : clientQueues) {
            auto snapshot = std::make_shared<Snapshot>();
            snapshot->controlEvent = EventType::GAME_START;
            q->push(snapshot);
        }

        // gameLoop acepta la queue compartida, el vector de queues privadas y el mutex de clientes
        gameloop = std::make_unique<GameLoop>(sharedQueue, clientQueues, clientHandlers, mtx, this);

        for (auto* handler : clientHandlers) {
            handler->setIsPlaying();
            gameloop->addPlayer(handler->getId(), handler->getCarId() + 1, handler->getPlayerName());
        }

        gameloop->start();
    }
}

void Game::addClientHandler(ClientHandler* client) {
    std::lock_guard<std::mutex> lock(mtx);
    clientHandlers.push_back(client);
    clientQueues.push_back(&client->getClientQueue());

    if (gameloop) {
        gameloop->addPlayer(client->getId(), client->getCarId() + 1, client->getPlayerName());
    }

    //se verifica si comienza la partida
    checkGameStart();
}

void Game::removeClientHandler(ClientHandler* client) {
    // std::lock_guard<std::mutex> lock(mtx);
    auto itH = std::find(clientHandlers.begin(), clientHandlers.end(), client);
    if (itH != clientHandlers.end()) clientHandlers.erase(itH);
    totalPlayers = std::max(0, totalPlayers - 1);

    auto itQ = std::find_if(clientQueues.begin(), clientQueues.end(),
                            [&](auto q){ return q == &client->getClientQueue(); });
    if (itQ != clientQueues.end()) {
        try { (*itQ)->close(); } catch(...) {}
        clientQueues.erase(itQ);
    }

    if (clientQueues.empty() && gameloop) {
        std::cout << "debug: no hay mas jugadores en la partida" << std::endl;
        gameloop->stop();
    }
}

// se cierran todas las queues
void Game::closeAllClientQueues() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto qptr : clientQueues) {
        if (qptr) try { qptr->close(); } catch(...) {}
    }
    try { sharedQueue.close(); } catch (...) {}
}

Game::~Game() {
    if (gameloop) {
        gameloop->stop();
        gameloop->join();
        gameloop.reset();
    }
    closeAllClientQueues();
    clientHandlers.clear();
}

std::string Game::getCreatorsName() {
    return gameCreator;
}

void Game::startGame() {
    gameStarted = true;
    checkGameStart();
}

bool Game::getGameStarted() {
    return gameStarted;
}
