#include "server_gameloop.h"

GameLoop::GameLoop(Queue<std::shared_ptr<Message>>& commandQueue,
                   std::vector<Queue<std::shared_ptr<Snapshot>>*> clientQueues)
    : running(false),
      commandQueue(commandQueue),
      clientQueues(clientQueues) {}

static constexpr uint32_t SPAWN_X = 90;
static constexpr uint32_t SPAWN_Y= 90;

uint32_t x = SPAWN_X;   // spawn coherente con el cliente (antes: 0)
uint32_t y = SPAWN_Y;   // spawn coherente con el cliente (antes: 0)

void GameLoop::run() {
    std::cout << "Arranco gameloop" << std::endl;
    running = true;

    // Enviar un snapshot inicial a todos los clientes para alinear posiciones
    auto initial = std::make_shared<Snapshot>();
    initial->posX = x;
    initial->posY = y;
    initial->controlEvent = EventType::NONE;
    {
        std::lock_guard<std::mutex> lock(qmtx);
        for (auto qptr : clientQueues) {
            if (qptr) {
                try { qptr->push(initial); std::cout << "Mande init - gameloop" << std::endl;} catch (const ClosedQueue&) {}
            }
        }
    }
    while (running) {
        processCommandQueue();
        std::this_thread::sleep_for(std::chrono::milliseconds(Constants::THREAD_SLEEP_MS));
    }
}

void GameLoop::processCommandQueue() {
    std::shared_ptr<Message> msg;
    while (commandQueue.try_pop(msg)) {
        if (!msg) continue;

        switch (msg->key) {
            case 'w': y--; break;
            case 's': y++; break;
            case 'a': x--; break;
            case 'd': x++; break;
        }

        auto snapshot = std::make_shared<Snapshot>();
        snapshot->posX = x;
        snapshot->posY = y;
        snapshot->controlEvent = EventType::NONE;

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

void GameLoop::stop() {
    running = false;
}

GameLoop::~GameLoop() {
    stop();
}
