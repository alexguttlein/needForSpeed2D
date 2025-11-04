#include "server_gameloop.h"

GameLoop::GameLoop(Queue<std::shared_ptr<Message>>& commandQueue,
                   std::vector<Queue<std::shared_ptr<Snapshot>>*> clientQueues)
    : running(false),
      commandQueue(commandQueue),
      clientQueues(clientQueues) {}

void GameLoop::run() {
    running = true;

    while (running) {
        processCommandQueue();
        std::this_thread::sleep_for(std::chrono::milliseconds(Constants::THREAD_SLEEP_MS));
    }
}

uint32_t x = 0;//para test
uint32_t y = 0;//para test
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
