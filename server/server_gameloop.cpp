#include "server_gameloop.h"

GameLoop::GameLoop(Queue<std::shared_ptr<Message>>& commandQueue,
                   std::vector<Queue<std::shared_ptr<Snapshot>>*> clientQueues)
    : running(false),
      commandQueue(commandQueue),
      clientQueues(clientQueues) {}


uint32_t x = 520;   // spawn coherente con el cliente (antes: 0)
uint32_t y = 240;   // spawn coherente con el cliente (antes: 0)

void GameLoop::run() {
    running = true;

    // Enviar un snapshot inicial a todos los clientes para alinear posiciones
    {
        auto initial = std::make_shared<Snapshot>();
        initial->posX = x;
        initial->posY = y;
        initial->controlEvent = EventType::NONE;
        std::lock_guard<std::mutex> lock(qmtx);
        for (auto qptr : clientQueues) {
            if (qptr) {
                try { qptr->push(initial); } catch (const ClosedQueue&) {}
            }
        }
    }

    while (running) {
        processCommandQueue();
        std::this_thread::sleep_for(std::chrono::milliseconds(Constants::THREAD_SLEEP_MS));
    }
}


// uint32_t x = 0;//para test
// uint32_t y = 0;//para test
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
