#include "server_gameloop.h"

GameLoop::GameLoop(Queue<std::shared_ptr<Message>>& commandQueue,
                   const std::vector<Queue<std::shared_ptr<Snapshot>>*> &clientQueues)
    : running(false),
      commandQueue(commandQueue),
      clientQueues(clientQueues) {}

void GameLoop::run() {
    running = true;

    static constexpr uint32_t SPAWN_X = 90;
    static constexpr uint32_t SPAWN_Y = 90;
    static constexpr uint32_t OFFSET = 50; // para separar autos al spawnear

    // inicializar posiciones de spawn para todos los jugadores
    uint32_t spawnX = SPAWN_X;
    uint32_t spawnY = SPAWN_Y;

    for (auto& [id, player] : players) {
        player.posX = spawnX;
        player.posY = spawnY;

        // siguiente jugador spawnea un poco más a la derecha, por ejemplo
        spawnX += OFFSET;
        if (spawnX > 600) {  // salto de línea si se sale del área
            spawnX = SPAWN_X;
            spawnY += OFFSET;
        }
    }

    // se crea snapshot inicial global
    auto initial = std::make_shared<Snapshot>();
    initial->controlEvent = EventType::NONE;
    initial->players.reserve(players.size());

    for (const auto& [pid, pstate] : players) {
        initial->players.push_back({
            pstate.playerId,
            pstate.posX,
            pstate.posY
        });
    }

    initial->playersSize = static_cast<uint32_t>(initial->players.size());

    // se envia snapshot a todos los clientes
    {
        std::lock_guard<std::mutex> lock(qmtx);
        for (auto qptr : clientQueues) {
            if (qptr) {
                try {
                    qptr->push(initial);
                } catch (const ClosedQueue&) {
                }
            }
        }
    }
    int testFinJuego = 100;
    while (running) {
        processCommandQueue();
        validateGameEnd(testFinJuego);
        testFinJuego--;
        std::this_thread::sleep_for(std::chrono::milliseconds(Constants::THREAD_SLEEP_MS));
    }
}

void GameLoop::processCommandQueue() {
    std::shared_ptr<Message> msg;
    while (commandQueue.try_pop(msg)) {
        if (!msg) continue;

        int id = msg->senderId;
        auto it = players.find(id);
        if (it == players.end()) continue;  // jugador desconocido
        auto& player = it->second;

        switch (msg->key) {
            case 'w': player.posY-=4; break;
            case 's': player.posY+=4; break;
            case 'a': player.posX-=4; break;
            case 'd': player.posX+=4; break;
        }

        // Crear snapshot con el estado global de todos los jugadores
        auto snapshot = std::make_shared<Snapshot>();
        snapshot->controlEvent = EventType::NONE;
        snapshot->playerId = id;  // quién generó este movimiento

        // llenar la lista de jugadores
        snapshot->players.reserve(players.size());
        for (const auto& [pid, pstate] : players) {
            snapshot->players.push_back({
                pstate.playerId,
                pstate.posX,
                pstate.posY
            });
        }
        snapshot->playersSize = static_cast<uint32_t>(snapshot->players.size());

        std::lock_guard<std::mutex> lock(qmtx);
        for (auto qptr : clientQueues) {
            if (qptr) {
                try {
                    qptr->push(snapshot);
                } catch (const ClosedQueue&) {
                    std::cout << "[GameLoop] cola cerrada al hacer push" << std::endl;
                }
            }
        }
    }
}

void GameLoop::addPlayer(int playerId) {
    static constexpr uint32_t SPAWN_X = 90;
    static constexpr uint32_t SPAWN_Y = 90;
    players[playerId] = {playerId, SPAWN_X, SPAWN_Y};
}

void GameLoop::stop() {
    running = false;
}

GameLoop::~GameLoop() {
    stop();
    // std::lock_guard<std::mutex> lock(qmtx);
    players.clear();
    clientQueues.clear();
}

void GameLoop::validateGameEnd(int counter) {
    if (counter == 0) {
        std::cout << "FIN DE LA CARRERA" << std::endl;
        stop();
    }
    std::cout << "TIME: " << counter << std::endl;
}