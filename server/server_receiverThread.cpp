#include "server_receiverThread.h"
#include "server_gameMonitor.h"
#include "server_clientHandler.h"

ReceiverThread::ReceiverThread(ServerProtocol& protocol,
    GameMonitor& gameMonitor, ClientHandler& clientHandler) :
    protocol(protocol), gameMonitor(gameMonitor),
    clientHandler(clientHandler), gameQueue(nullptr) {
}

void ReceiverThread::lobbyCommands(Message msg) {
    if (msg.code == Constants::CREATE_GAME) {
        clientHandler.setPlayerName(msg.stringValue); //se asigna el nombre de jugador
        int newId = gameMonitor.createGame(msg.stringValue);

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

        std::vector<uint8_t> buffer;
        buffer.push_back(Constants::TYPE_CONTROL);
        buffer.push_back(Constants::CREATE_JOIN_ACCEPTED);
        protocol.addIntToUint8tVector(buffer, clientHandler.getId());
        protocol.sendCreateJoinAccepted(buffer);

    } else if (msg.code == Constants::LIST_GAMES) {
        // construir vector de pares (id, totalPlayers)
        std::vector<uint8_t> buffer;
        auto gamesSnapshot = gameMonitor.listGames();
        for (auto& [id, total] : gamesSnapshot) {
            // id y totalPlayers en big endian
            uint32_t idBE = htonl(id);
            uint32_t totalBE = htonl(total);
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&idBE), reinterpret_cast<uint8_t*>(&idBE)+4);
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&totalBE), reinterpret_cast<uint8_t*>(&totalBE)+4);

            //recupero el nombre del creador de cada partida
            std::string gameCreator = gameMonitor.getGameCreator(id);

            //se agrega el tamanio del nombre
            uint16_t lenBE = htons(gameCreator.size());
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&lenBE), reinterpret_cast<uint8_t*>(&lenBE)+2);

            //se agrerga el nombre del creador
            if (!gameCreator.empty()) {
                buffer.insert(buffer.end(), gameCreator.begin(), gameCreator.end());
            }
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
            clientHandler.setPlayerName(msg.stringValue); //se asigna el nombre de jugador
            std::vector<uint8_t> buffer;
            buffer.push_back(Constants::TYPE_CONTROL);
            buffer.push_back(Constants::CREATE_JOIN_ACCEPTED);
            protocol.addIntToUint8tVector(buffer, clientHandler.getId());
            protocol.sendCreateJoinAccepted(buffer);
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
    while (keepRunning) {
        if (protocol.isConnectionClosed()) {
            keepRunning = false;
            return;
        }

        Message msg;
        try {
            msg = protocol.receiveMessage();
        } catch (const std::exception&) {
            keepRunning = false;
            break;
        }

        // Si todavía no tiene partida asignada
        if (gameQueue == nullptr) {
            lobbyCommands(msg);
            continue;
        }

        if (!gameQueue) {
            std::cerr << "Error: mensaje recibido pero el cliente "
                      << clientHandler.getId()
                      << " no tiene gameQueue asignada." << std::endl;
            continue;
        }

        // si ya está en una partida encola el mensaje
        // le agrega el id del cliente para poder identificar quien envio el mensaje
        auto msgPtr = std::make_shared<Message>(msg);
        msgPtr->senderId = clientHandler.getId();

        // agrega el mensaje a la cola compartida
        gameQueue->push(msgPtr);
    }
    stop();
}

void ReceiverThread::stop() {
    keepRunning = false;
    protocol.closeSocket();
}

ReceiverThread::~ReceiverThread() {
    // if (keepRunning) ReceiverThread::stop();
    // protocol.closeSocket();
    keepRunning = false;
    try { protocol.closeSocket(); } catch(...) {}
}