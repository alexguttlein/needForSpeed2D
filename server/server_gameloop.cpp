#include "server_gameloop.h"


GameLoop::GameLoop(Queue<std::string>& commandQueue, MonitorClients& clients)
    : running(false), clients(clients), commandQueue(commandQueue) {}


void GameLoop::run() {
    running = true;

    while (running) {
   
        clients.killDisconnectedClients();
        processCommandQueue();
        broadcastSnapshots();
        std::this_thread::sleep_for(std::chrono::milliseconds(Constants::THREAD_SLEEP_MS)); 
    }
}


void GameLoop::processCommandQueue() {
    std::string command;
    while (commandQueue.try_pop(command)) {
        std::cout << "Comando recibido: " << command << std::endl;
    }
}

void GameLoop::broadcastSnapshots() {
    clients.forEachClient([this](ClientHandler& client){
        Snapshot snapshot;
        if (client.getSnapshots().popSnapshot(snapshot)) {
            auto msg = std::make_shared<Message>();
            msg->code = 1;      // codigo de respuesta de snapshot? 
            msg->car = snapshot.car;

            clients.broadcastToAllClients(msg);
        }
    });
}



void GameLoop::stop() {
    running = false;
}


GameLoop::~GameLoop() {
    stop();
}
