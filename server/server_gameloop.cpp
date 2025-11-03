#include "server_gameloop.h"


GameLoop::GameLoop(Queue<std::string>& commandQueue)
    : running(false), commandQueue(commandQueue), gameLogic(), snapshots() {}


void GameLoop::run() {

    const float dt = 0.016f; // por ahora
    running = true;

    while (running) {
        processCommandQueue();
        gameLogic.update(dt);
        saveSnapshots();
        // std::this_thread::sleep_for(std::chrono::milliseconds(Constants::THREAD_SLEEP_MS));
    }
}


void GameLoop::processCommandQueue() {
    std::string command;
    while (commandQueue.try_pop(command)) {
        std::cout << "Comando recibido: " << command << std::endl;
        gameLogic.processCommand(1, command);  // Por ahora, todos los comandos van al auto con ID 1
    }
}


void GameLoop::saveSnapshots() {
    GameSnapshot snapshot = gameLogic.getSnapshot();
    std::cout << "Snapshot creado con " << snapshot.num_cars << " autos." << std::endl;
    std::cout << "Posiciones de los autos:" << std::endl;
    for (uint8_t i = 0; i < snapshot.num_cars; ++i) {
        const CarStateDTO& carState = snapshot.car_states[i];
        std::cout << "  Auto ID " << static_cast<int>(carState.car_id)
                  << ": Pos(" << carState.position.x << ", " << carState.position.y << ")\n";
    }
    snapshots.addSnapshot(snapshot);
}



void GameLoop::stop() {
    running = false;
}


GameLoop::~GameLoop() {
    stop();
}
