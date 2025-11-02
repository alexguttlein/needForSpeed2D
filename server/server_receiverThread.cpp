#include "server_receiverThread.h"
#include "server_clientHandler.h"

ReceiverThread::ReceiverThread(ServerProtocol& protocol,
    GameMonitor& gameMonitor, ClientHandler& clientHandler) :
    protocol(protocol), gameMonitor(gameMonitor),
    clientHandler(clientHandler), gameQueue(nullptr), keepRunning(true) {
}

void ReceiverThread::lobbyCommands(Message msg) {
    if (msg.code == Constants::CREATE_GAME) {
        std::cout << "Debug: se va a crear partida" << std::endl;
        int newId = gameMonitor.createGame();
        std::cout << "Debug: se crea partida con id " << newId << std::endl;
        gameQueue = &gameMonitor.getGameQueue(newId);
        clientHandler.assignGameQueue(*gameQueue, newId);
        std::cout << "Debug: Client " << clientHandler.getId() << " created game " << newId << std::endl;
        protocol.sendControl(Constants::CREATE_JOIN_ACCEPTED);
    } else if (msg.code == Constants::JOIN_GAME) {
        int joinId = msg.intValue;

        Queue<std::shared_ptr<Message>>* outQueue = nullptr;
        bool canJoin = gameMonitor.tryJoinGame(joinId, outQueue);

        if (!canJoin) {
            // error: informar al cliente que no pudo unirse
            std::cerr << "Debug: Client " << clientHandler.getId() << " failed to join game " << joinId << std::endl;
            protocol.sendControl(Constants::JOIN_REJECTED);
            return;
        } else {
            protocol.sendControl(Constants::CREATE_JOIN_ACCEPTED);
        }

        // exitoso: asignar queue y marcar currentGameId
        gameQueue = outQueue;
        clientHandler.assignGameQueue(*gameQueue, joinId);
        std::cout << "Debug: Client " << clientHandler.getId() << " joined game " << joinId << std::endl;
    } else {
        std::cerr << "Invalid command before joining a game." << std::endl;
        return;
    }
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
            lobbyCommands(msg);
            continue;
        }

        // Si ya está en una partida
        gameQueue->push(std::make_shared<Message>(msg));

        std::shared_ptr<Message> poped = gameQueue->pop();
        std::cout << "Debug: se recibe " << poped->key << std::endl;
    }
}
