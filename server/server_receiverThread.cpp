#include "server_receiverThread.h"
#include "server_clientHandler.h"

ReceiverThread::ReceiverThread(ServerProtocol& protocol,
    GameMonitor& gameMonitor, ClientHandler& clientHandler) :
    protocol(protocol), gameMonitor(gameMonitor),
    clientHandler(clientHandler), gameQueue(nullptr), keepRunning(true) {
}

void ReceiverThread::run() {
    std::cout << "Debug: ReceiverThread::run()" << std::endl;
    while (keepRunning) {
        if (protocol.isConnectionClosed()) {
            keepRunning = false;
            return;
        }

        Message msg = protocol.receiveMessage();
        // Si todavía no tiene partida asignada
        if (gameQueue == nullptr) {
            if (msg.code == 0x09) { // TODO: recuperar instruccion valida CREATE_GAME
                std::cout << "Debug: se va a crear partida" << std::endl;
                int newId = gameMonitor.createGame();
                std::cout << "se crea partida con id " << newId << std::endl;
                gameQueue = &gameMonitor.getGameQueue(newId);
                clientHandler.assignGameQueue(*gameQueue);
                std::cout << "Client " << clientHandler.getId() << " created game " << newId << std::endl;
            } else if (msg.code == 0x10) { // TODO: recuperar instruccion valida JOIN_GAME
                int joinId = msg.value;
                gameQueue = &gameMonitor.getGameQueue(joinId);
                clientHandler.assignGameQueue(*gameQueue);
                std::cout << "Client " << clientHandler.getId() << " joined game " << joinId << std::endl;
            } else {
                std::cerr << "Invalid command before joining a game." << std::endl;
                continue;
            }
            continue;
        }

        // Si ya está en una partida
        gameQueue->push(std::make_shared<Message>(msg));

        std::shared_ptr<Message> poped = gameQueue->pop();
        std::cout << "Debug: se recibe " << poped->key << std::endl;
    }
}
