#include "server_gameloop.h"

GameLoop::GameLoop(Queue<std::shared_ptr<Message>>& commandQueue,
                   std::vector<Queue<std::shared_ptr<Snapshot>>*> clientQueues)
    : running(false),
      commandQueue(commandQueue),
      clientQueues(clientQueues) {}

void GameLoop::run() {

    std::cout << "Arranco gameloop" << std::endl;
    running = true;

    const std::chrono::milliseconds targetTickTime(Constants::THREAD_SLEEP_MS);

    while (running) {

        auto start = std::chrono::steady_clock::now();
        processCommandQueue();
        gameLogic.update(); 

        std::shared_ptr<Snapshot> snapshotToSend = gameLogic.getSnapshot( EventType::NONE);
        {
            std::lock_guard<std::mutex> lock(qmtx);
            for (auto qptr : clientQueues) {
                if (qptr) {
                    try {
                        qptr->push(snapshotToSend);
                    } catch (const ClosedQueue&) {
                        std::cout << "[GameLoop] Cola cerrada al enviar snapshot" << std::endl;
                    }
                }
            }
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(Constants::THREAD_SLEEP_MS));
        auto end = std::chrono::steady_clock::now();
        auto processingDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        // 💡 3. Dormir el tiempo restante (Delta Time Compensation)
        if (processingDuration < targetTickTime) {
            auto sleepDuration = targetTickTime - processingDuration;
            std::this_thread::sleep_for(sleepDuration);
        }
    }
}

void GameLoop::processCommandQueue() {
    std::shared_ptr<Message> msg;
    while (commandQueue.try_pop(msg)) {
        if (!msg) continue;
        bool is_pressed = (msg->code & 0x80) != 0;  
        char key_char = msg->key; 
        gameLogic.processCommand(msg->senderId, std::string(1, key_char), is_pressed);
    }
}

void GameLoop::addPlayer(int playerId) {
    static constexpr int DEFAULT_CAR_TYPE = 1;
    gameLogic.addCar(playerId, DEFAULT_CAR_TYPE);
}


void GameLoop::stop() {
    running = false;
}

GameLoop::~GameLoop() {
    stop();
}
