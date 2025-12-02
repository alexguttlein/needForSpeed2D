#include "server_gameloop.h"

GameLoop::GameLoop(Queue<std::shared_ptr<Message>>& commandQueue,
                   std::vector<Queue<std::shared_ptr<Snapshot>>*>& clientQueues,
                   std::vector<ClientHandler*>& clientHandlers,
                   std::mutex& clientListMutex, Game* parentGame)
    : running(false),
      commandQueue(commandQueue),
      clientQueues(clientQueues),
      clientHandlers(clientHandlers),
      clientListMutex(clientListMutex),
      parentGame(parentGame) {}

void GameLoop::run() {

    std::cout << "Arranco gameloop" << std::endl;
    running = true;

    const std::chrono::milliseconds rate = std::chrono::milliseconds(1000 / Constants::TICKS_PER_SECOND);
    auto t1 = std::chrono::steady_clock::now(); // Tiempo inicial t1
    int it = 0;

    while (running) {

        simulateGame(it);

        auto t2 = std::chrono::steady_clock::now(); // Tiempo después de procesar el frame

        auto rest_duration = t1 + rate - t2;
        long long rest = std::chrono::duration_cast<std::chrono::nanoseconds>(rest_duration).count();

        if (rest < 0) {
            long long behind = -rest; // Tiempo que estamos atrasados (positivo)

            long long rate_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(rate).count();

            long long sleep_to_sync = rate_ns - (behind % rate_ns); // Tiempo para sincronizarse

            long long lost = behind + sleep_to_sync;  // Tiempo total perdido

            t1 += std::chrono::nanoseconds(lost); // Avanzar t1 para compensar el retraso

            it += lost / rate_ns; // para debug
            //std::cout << "Dropping " << it << " ticks" << std::endl;

            std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_to_sync)); // Dormir para sincronizarse
        } else {
            std::this_thread::sleep_for(std::chrono::nanoseconds(rest)); // camino feliz
        }
        t1 += rate;
        it++;
    }
    std::cout << "Finalizo el gameloop" << std::endl;
}

void GameLoop::processCommandQueue() {
    std::shared_ptr<Message> msg;
    while (commandQueue.try_pop(msg)) {
        if (!msg) continue;
        
        // Verificar si es un comando de cheat
        if (msg->code == Constants::CHEAT_COMMAND) {
            gameLogic.processCheat(msg->senderId, msg->stringValue);
        } else {
            bool is_pressed = (msg->code & 0x80) != 0;
            char key_char = msg->key;
            gameLogic.processCommand(msg->senderId, std::string(1, key_char), is_pressed);
        }
    }
}


void GameLoop::addPlayer(int playerId, int carId, const std::string& playerName) {
    gameLogic.addCar(playerId, carId, playerName);
}


void GameLoop::simulateGame(int currentTick) {
    processCommandQueue();
    gameLogic.update(currentTick);

    std::shared_ptr<Snapshot> snapshotToSend = gameLogic.getSnapshot( EventType::NONE);
    {
        std::lock_guard<std::mutex> lock(clientListMutex);
        std::vector<size_t> disconnectedPlayers{};

        for (size_t i = 0; i < clientQueues.size(); i++) {
            auto* qptr = clientQueues[i];
            auto* handler = clientHandlers[i];

            if (!qptr) continue;

            if (!qptr->try_push(snapshotToSend)) {
                std::cout << "Debug: Cliente " << handler->getId()
                          << " no recibe snapshot. Posible desconexión." << std::endl;

                // se verifica si el cliente está desconectado
                if (!handler->isConnected() || !handler->isAlive()) {

                    std::cout << "[GameLoop] Cliente " << handler->getId()
                              << "  desconectado. Eliminando del juego.\n";

                    disconnectedPlayers.push_back(i); //se agregan los desconectado a la lista
                }
            }
        }

        //se eliminan los clientes desconectados
        if (!disconnectedPlayers.empty()) {
            for (int idx : disconnectedPlayers) {
                parentGame->removeClientHandler(clientHandlers[idx]);
            }
        }
    }
}


void GameLoop::stop() {
    running = false;
}

GameLoop::~GameLoop() {
    stop();
}