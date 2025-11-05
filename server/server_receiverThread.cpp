#include "server_receiverThread.h"
#include "server_gameMonitor.h"
#include "server_clientHandler.h"

ReceiverThread::ReceiverThread(ServerProtocol& protocol,
    GameMonitor& gameMonitor, ClientHandler& clientHandler) :
    protocol(protocol), gameMonitor(gameMonitor),
    clientHandler(clientHandler), gameQueue(nullptr), keepRunning(true) {
}

void ReceiverThread::lobbyCommands(Message msg) {
    if (msg.code == Constants::CREATE_GAME) {
        int newId = gameMonitor.createGame();
        std::cout << "Debug: se crea partida con id " << newId << std::endl;
        gameQueue = &gameMonitor.getGameQueue(newId);
        clientHandler.assignGameQueue(*gameQueue, newId);
        std::cout << "Debug: Client " << clientHandler.getId() << " created game " << newId << std::endl;

        // registrar Cliente en la partida para que Game conozca su queue privada
        // TODO: encapsular metodo, se usa tambien en join
        bool regOk = gameMonitor.registerClientToGame(newId, &clientHandler);
        if (!regOk) {
            std::cerr << "Error: no se pudo registrar client en game " << newId << std::endl;
            protocol.sendControl(Constants::JOIN_REJECTED);
            return;
        }
        protocol.sendControl(Constants::CREATE_JOIN_ACCEPTED);

    } else if (msg.code == Constants::LIST_GAMES) {
        // construir vector de pares (id, totalPlayers)
        std::cout << "Debug: List games: se arma lista de partidas" << std::endl;
        std::vector<uint8_t> buffer;
        auto gamesSnapshot = gameMonitor.listGames();
        std::cout << "Debug: List games: se armó la lista de partidas" << std::endl;
        for (auto& [id, total] : gamesSnapshot) {
            // id y totalPlayers en big endian
            uint32_t idBE = htonl(id);
            uint32_t totalBE = htonl(total);
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&idBE), reinterpret_cast<uint8_t*>(&idBE)+4);
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&totalBE), reinterpret_cast<uint8_t*>(&totalBE)+4);
        }

        // se envia lista de partidas
        uint8_t type = Constants::TYPE_GAME_LIST;
        protocol.sendGamesList(type, buffer);

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

        // registrar Cliente en la partida para que Game conozca su queue privada
        bool regOk = gameMonitor.registerClientToGame(joinId, &clientHandler);
        if (!regOk) {
            std::cerr << "Error: no se pudo registrar client en game " << joinId << std::endl;
            protocol.sendControl(Constants::JOIN_REJECTED);
            return;
        }
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

        // si ya está en una partida encola el mensaje
        // en la queue compartida de la partida
        gameQueue->push(std::make_shared<Message>(msg));
    }
}
