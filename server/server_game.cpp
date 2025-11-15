#include "server_game.h"
#include "server/server_gameloop.h"

#include <iostream>

Game::Game(int gameId) :
    totalPlayers(0),
    gameId(gameId),
    sharedQueue(Constants::GAME_QUEUE_MAXSIZE),
    clientQueues(),
    clientHandlers(),
    gameloop(nullptr) {}

Queue<std::shared_ptr<Message>>& Game::getSharedQueue() {
    return sharedQueue;
}

void Game::addClientHandler(ClientHandler* client) {
    std::lock_guard<std::mutex> lock(mtx);
    clientHandlers.push_back(client);
    clientQueues.push_back(&client->getClientQueue());

    if (gameloop) {
        gameloop->addPlayer(client->getId());
    }

    // si alcanzamos el número de jugadores, arrancamos GameLoop si aún no arrancó
    if ((int)clientQueues.size() >= Constants::MAX_PLAYERS_IN_GAME && !gameloop) {
        // como se conectaron todos los usuarios,
        // gameLoop ahora acepta la queue compartida y el vector de queues privadas
        gameloop = std::make_unique<GameLoop>(sharedQueue, clientQueues);
        for (auto* handler : clientHandlers)
            gameloop->addPlayer(handler->getId());
        gameloop->start();
    }
}

void Game::removeClientHandler(ClientHandler* client) {

    std::lock_guard<std::mutex> lock(mtx);
    auto itH = std::find(clientHandlers.begin(), clientHandlers.end(), client);
    if (itH != clientHandlers.end()) clientHandlers.erase(itH);
    totalPlayers = std::max(0, totalPlayers - 1);

    auto itQ = std::find_if(clientQueues.begin(), clientQueues.end(),
                            [&](auto q){ return q == &client->getClientQueue(); });
    if (itQ != clientQueues.end()) {
        try { (*itQ)->close(); } catch(...) {}
        clientQueues.erase(itQ);
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

    // for (auto ch : clientHandlers) {
    //     if (ch) {
    //         try { ch->shutdown(); } catch(...) {}
    //     }
    // }
    closeAllClientQueues();
    clientHandlers.clear();
}
