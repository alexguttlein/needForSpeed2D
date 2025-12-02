#include "client_protocol.h"

ClientProtocol::ClientProtocol(const char* host, const char* port) :
    socket(host, port),
    isClosed(false) {}

ClientProtocol::~ClientProtocol() {
    if (!isClosed && !socket.is_stream_recv_closed()) {
        socket.shutdown(SHUT_RDWR);
        socket.close();
        isClosed = true;
    }
}

void ClientProtocol::sendKey(const SDL_KeyCode input, bool isPressed) {
    try {
        auto key = sdlToKey(input);
        uint8_t msg = CommandConstants::keyToBit(key);
        if (isPressed) {
            msg |= 0x80; // Asume BIT_KEY_DOWN = 0x80
        }
        socket.sendall(&msg, sizeof(msg));
    } catch (const std::exception& e) {
        // tecla no reconocida, no enviamos nada
    }
}

CommandConstants::Key ClientProtocol::sdlToKey(const SDL_KeyCode input) {
    switch (input) {
        case SDLK_w: return CommandConstants::W;
        case SDLK_a: return CommandConstants::A;
        case SDLK_s: return CommandConstants::S;
        case SDLK_d: return CommandConstants::D;
        case SDLK_1: return CommandConstants::KEY_1;
        case SDLK_2: return CommandConstants::KEY_2;
        case SDLK_3: return CommandConstants::KEY_3;
        case SDLK_4: return CommandConstants::KEY_4;
        default: throw std::invalid_argument("Tecla no válida");
    }
}

uint32_t ClientProtocol::readUInt32(const std::vector<uint8_t>& buffer, size_t& offset) {
    uint32_t be_value = 0;
    std::memcpy(&be_value, &buffer[offset], sizeof(be_value));
    offset += sizeof(be_value);

    uint32_t value = ntohl(be_value);  // Network → Host
    return value;
}

bool ClientProtocol::sendLobbyOption(const std::string& input, const std::string& playerName, const int& carId) {
    if (socket.is_stream_send_closed()) return false;
    std::istringstream iss(input);
    std::string command;
    iss >> command;

    uint32_t carIdBE = htonl(static_cast<uint32_t>(carId));

    if (command == Constants::INPUT_CREAR) {
        uint8_t msg = Constants::CREATE_GAME;
        socket.sendall(&msg, sizeof(msg));
        sendString(playerName); //se envia nombre de usuario
        socket.sendall(&carIdBE, sizeof(carIdBE)); //se envia id de auto elegido
        return true;
    } else if (command == Constants::INPUT_UNIRSE) {
        uint8_t msg = Constants::JOIN_GAME;
        socket.sendall(&msg, sizeof(msg));

        uint16_t matchId;
        if (!(iss >> matchId)) {
            std::cerr << "Debes ingresar un ID de partida.\n";
            return false;
        }

        // se convierte a big endian (2 bytes)
        uint16_t matchIdBE = htons(matchId);
        socket.sendall(reinterpret_cast<uint8_t*>(&matchIdBE), sizeof(matchIdBE));
        sendString(playerName); //se envia nombre de usuario
        socket.sendall(&carIdBE, sizeof(carIdBE)); //se envia id de auto elegido
        return true;

    } else if (command == Constants::INPUT_LISTAR) {
        uint8_t msg = Constants::LIST_GAMES;
        socket.sendall(&msg, sizeof(msg));
        return false; // sigue en el lobby

    } else if (command == Constants::INPUT_START_GAME) {
        uint8_t msg = Constants::GAME_START;
        socket.sendall(&msg, sizeof(msg));
        socket.sendall(&carIdBE, sizeof(carIdBE)); //se envia id de la partida
        return true;
    }

    std::cerr << "Opción inválida. Usa 'crear' o 'unirse <id>'" << std::endl;
    return false;
}


void ClientProtocol::sendCheat(const std::string& cheatCode) {
    if (socket.is_stream_send_closed()) return;
    
    try {
        // Enviar byte de comando cheat
        uint8_t msg = Constants::CHEAT_COMMAND;
        socket.sendall(&msg, sizeof(msg));
        
        // Enviar el código del cheat como string
        sendString(cheatCode);
        
        std::cout << "[ClientProtocol] Cheat enviado: " << cheatCode << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ClientProtocol] Error enviando cheat: " << e.what() << std::endl;
    }
}


std::optional<Snapshot> ClientProtocol::receiveSnapshotFromServer() {
    // se recibio un Snapshot
    Snapshot snapshot{};
    // leer controlEvent
    uint8_t controlEventByte = 0;
    socket.recvall(&controlEventByte, sizeof(controlEventByte));
    snapshot.controlEvent = static_cast<EventType>(controlEventByte);

    //se lee el id del jugador/cliente
    uint32_t playerIdBE = 0;
    socket.recvall(&playerIdBE, sizeof(playerIdBE));
    snapshot.playerId = ntohl(playerIdBE);

    //se lee la cantidad de jugadoores a recibir
    uint32_t playersSizeBE = 0;
    socket.recvall(&playersSizeBE, sizeof(playersSizeBE));
    snapshot.playersSize = static_cast<uint32_t>(ntohl(playersSizeBE));

    for (uint32_t i = 0; i < snapshot.playersSize; i++) {
        CarStateDTO dto{};

        uint32_t idBE = 0;
        socket.recvall(&idBE, sizeof(idBE));
        dto.car_id = static_cast<int>(ntohl(idBE));

        uint32_t carTypeIdBE = 0;
        socket.recvall(&carTypeIdBE, sizeof(carTypeIdBE));
        dto.car_type_id = static_cast<int>(ntohl(carTypeIdBE));

        uint32_t upgradeIdBE = 0;
        socket.recvall(&upgradeIdBE, sizeof(upgradeIdBE));
        dto.currentUpgradeId = static_cast<int>(ntohl(upgradeIdBE));

        uint32_t healthBE = 0;
        socket.recvall(&healthBE, sizeof(healthBE));

        uint32_t healthHost = ntohl(healthBE);
        dto.health = *reinterpret_cast<float*>(&healthHost);

        uint32_t speedBE = 0;
        socket.recvall(&speedBE, sizeof(speedBE));

        uint32_t speedHost = ntohl(speedBE);
        dto.speed = *reinterpret_cast<float*>(&speedHost);


        uint32_t posXBE = 0;
        socket.recvall(&posXBE, sizeof(posXBE));
        uint32_t xHost = ntohl(posXBE);
        dto.position.x = *reinterpret_cast<float*>(&xHost);


        uint32_t posYBE = 0;
        socket.recvall(&posYBE, sizeof(posYBE));
        uint32_t yHost = ntohl(posYBE);
        dto.position.y = *reinterpret_cast<float*>(&yHost);


        uint32_t angleXBE = 0;
        socket.recvall(&angleXBE, sizeof(angleXBE));
        uint32_t angleXHost = ntohl(angleXBE);
        dto.angle.x = *reinterpret_cast<float*>(&angleXHost);


        uint32_t angleYBE = 0;
        socket.recvall(&angleYBE, sizeof(angleYBE));
        uint32_t angleYHost = ntohl(angleYBE);
        dto.angle.y = *reinterpret_cast<float*>(&angleYHost);

        snapshot.cars.push_back(dto);
    }

        // raceStates
        uint32_t raceCountBE = 0;
        socket.recvall(&raceCountBE, sizeof(raceCountBE));
        uint32_t raceCount = ntohl(raceCountBE);

        snapshot.raceStates.clear();
        snapshot.raceStates.reserve(raceCount);

        for (uint32_t r = 0; r < raceCount; ++r) {
            RaceStateDTO rs{};

            // playerName
            uint16_t nameLenBE = 0;
            socket.recvall(&nameLenBE, sizeof(nameLenBE));
            uint16_t nameLen = ntohs(nameLenBE);
            rs.playerName.resize(nameLen);
            socket.recvall(rs.playerName.data(), nameLen);

            // playerId
            uint32_t plyrIdBE = 0;
            socket.recvall(&plyrIdBE, sizeof(plyrIdBE));
            rs.playerId = static_cast<int>(ntohl(plyrIdBE));

            // timeLeftRace
            uint16_t timeLenBE = 0;
            socket.recvall(&timeLenBE, sizeof(timeLenBE));
            uint16_t timeLen = ntohs(timeLenBE);
            rs.timeLeftRace.resize(timeLen);
            socket.recvall(rs.timeLeftRace.data(), timeLen);

            // currentRaceId
            uint32_t currentRaceIdBE = 0;
            socket.recvall(&currentRaceIdBE, sizeof(currentRaceIdBE));
            rs.currentRaceId = static_cast<int>(ntohl(currentRaceIdBE));

            // checkpointsSize
            uint32_t checkpointsSizeBE = 0;
            socket.recvall(&checkpointsSizeBE, sizeof(checkpointsSizeBE));
            rs.checkpointsSize = static_cast<int>(ntohl(checkpointsSizeBE));

            // nextCheckpoint
            uint32_t nextCheckpointXBE = 0;
            socket.recvall(&nextCheckpointXBE, sizeof(nextCheckpointXBE));
            uint32_t nextCheckpointXHost = ntohl(nextCheckpointXBE);
            rs.nextCheckpoint.x = *reinterpret_cast<float*>(&nextCheckpointXHost);

            uint32_t nextCheckpointYBE = 0;
            socket.recvall(&nextCheckpointYBE, sizeof(nextCheckpointYBE));
            uint32_t nextCheckpointYHost = ntohl(nextCheckpointYBE);
            rs.nextCheckpoint.y = *reinterpret_cast<float*>(&nextCheckpointYHost);

            // hints
            uint32_t hintsSizeBE = 0;
            socket.recvall(&hintsSizeBE, sizeof(hintsSizeBE));
            uint32_t hintsSize = ntohl(hintsSizeBE);

            for (uint32_t i = 0; i < hintsSize; i++) {
                Vector2D<float> hint;
                uint32_t hintXBE = 0;
                socket.recvall(&hintXBE, sizeof(hintXBE));
                uint32_t hintXHost = ntohl(hintXBE);
                hint.x = *reinterpret_cast<float*>(&hintXHost);

                uint32_t hintYBE = 0;
                socket.recvall(&hintYBE, sizeof(hintYBE));
                uint32_t hintYHost = ntohl(hintYBE);
                hint.y = *reinterpret_cast<float*>(&hintYHost);

                rs.currentHints.push_back(hint);
            }

            // hasFinished
            uint8_t hasFinishedByte = 0;
            socket.recvall(&hasFinishedByte, sizeof(hasFinishedByte));
            rs.hasFinished = static_cast<bool>(hasFinishedByte);

            // finishPosition
            uint32_t finishPositionBE = 0;
            socket.recvall(&finishPositionBE, sizeof(finishPositionBE));
            rs.finishPosition = static_cast<int>(ntohl(finishPositionBE));

            snapshot.raceStates.push_back(std::move(rs));
        }
        // Flag de fin de carrera
        uint8_t raceFinishedByte = 0;
        socket.recvall(&raceFinishedByte, sizeof(raceFinishedByte));
        snapshot.raceFinished = static_cast<bool>(raceFinishedByte);

        // Leer tiempos de llegada, en el mismo orden que raceStates
        for (uint32_t r = 0; r < raceCount; ++r) {
            uint32_t timeBE = 0;
            socket.recvall(&timeBE, sizeof(timeBE));
            uint32_t timeHost = ntohl(timeBE);
            float timeSeconds = *reinterpret_cast<float*>(&timeHost);
            snapshot.raceStates[r].finishTimeSeconds = timeSeconds;
        }


        // Flag de fin de juego
        uint8_t gameFinishedByte = 0;
        socket.recvall(&gameFinishedByte, sizeof(gameFinishedByte));
        snapshot.gameFinished = static_cast<bool>(gameFinishedByte);

        // 🟢 Leer TAMAÑO del leaderboard
        uint32_t leaderboardSizeBE = 0;
        socket.recvall(&leaderboardSizeBE, sizeof(leaderboardSizeBE));
        uint32_t leaderboardSize = ntohl(leaderboardSizeBE);

        // Leer leaderboard final
        // 🟢 Usar el tamaño leído, NO playersSizeBE
        for (uint32_t r = 0; r < leaderboardSize; ++r) {
            PlayerTime pt{};
            uint32_t plyrIdBE = 0;
            socket.recvall(&plyrIdBE, sizeof(plyrIdBE));
            pt.playerId = static_cast<int>(ntohl(plyrIdBE));

            uint32_t finishTimeBE = 0;
            socket.recvall(&finishTimeBE, sizeof(finishTimeBE));
            uint32_t finishTimeHost = ntohl(finishTimeBE);
            pt.finishTime = *reinterpret_cast<float*>(&finishTimeHost);

            // playerName
            uint16_t nameLenBE = 0;
            socket.recvall(&nameLenBE, sizeof(nameLenBE));
            uint16_t nameLen = ntohs(nameLenBE);
            pt.playerName.resize(nameLen);
            socket.recvall(pt.playerName.data(), nameLen);

            snapshot.leaderboards.push_back(pt);
        }

        // Leer eventos de colisión
        uint32_t collisionCountBE = 0;
        socket.recvall(&collisionCountBE, sizeof(collisionCountBE));
        uint32_t collisionCount = ntohl(collisionCountBE);
        
        for (uint32_t c = 0; c < collisionCount; ++c) {
            uint32_t playerIdBE = 0;
            socket.recvall(&playerIdBE, sizeof(playerIdBE));
            int playerId = static_cast<int>(ntohl(playerIdBE));
            
            uint8_t collisionTypeByte = 0;
            socket.recvall(&collisionTypeByte, sizeof(collisionTypeByte));
            EventType collisionType = static_cast<EventType>(collisionTypeByte);
            
            snapshot.collisions.emplace_back(playerId, collisionType);
        }

        return snapshot;
    }

std::optional<Snapshot> ClientProtocol::receiveControlFromServer() {
    // se recibio un codigo de control
    uint8_t code;
    socket.recvall(&code, sizeof(code));

    if (code == Constants::CREATE_JOIN_ACCEPTED) {
        //se recibe playerId
        uint32_t playerIdBE = 0;
        socket.recvall(&playerIdBE, sizeof(playerIdBE));
        int selfId = ntohl(playerIdBE);

        uint32_t gameIdBE = 0;
        socket.recvall(&gameIdBE, sizeof(gameIdBE));
        int gameId = ntohl(gameIdBE);

        Snapshot snapshot{};
        snapshot.controlEvent = EventType::CREATE_JOIN_ACCEPTED;
        snapshot.playerId = selfId;
        snapshot.gameId = gameId;

        return snapshot;
    } else if (code == Constants::JOIN_REJECTED) {
        Snapshot snapshot{};
        snapshot.controlEvent = EventType::JOIN_REJECTED;
        return snapshot;
    } else if (code == Constants::GAME_START) {
        Snapshot snapshot{};
        snapshot.controlEvent = EventType::GAME_START;
        return snapshot;
    } else if (code == Constants::SERVER_DISCONNECTED) {
        std::cout << "debug: client protocol -> se desconecto el server" << std::endl;
        Snapshot snapshot{};
        snapshot.controlEvent = EventType::SERVER_DISCONNECTED;
        return snapshot;
    } else if (code == Constants::PLAYER_COUNT_UPDATE) {
        Snapshot snapshot{};
        snapshot.controlEvent = EventType::PLAYER_COUNT_UPDATE;
        uint32_t playerCountBE = 0;
        socket.recvall(&playerCountBE, sizeof(playerCountBE));
        int playerCount = ntohl(playerCountBE);
        snapshot.gameId = playerCount;
        return snapshot;
    }
    else {
        std::cerr << "Código de control recibido: " << std::hex << (int)code << std::endl;
    }
    return std::nullopt;
}

std::optional<Snapshot> ClientProtocol::receiveGameListFromServer() {
    // Leer tamaño de la lista (4 bytes)
    uint32_t sizeBE = 0;
    if (socket.recvall(&sizeBE, sizeof(sizeBE)) <= 0) return std::nullopt;
    uint32_t listSize = ntohl(sizeBE);

    // Leer la lista completa
    std::vector<uint8_t> data(listSize);
    if (socket.recvall(data.data(), listSize) <= 0) return std::nullopt;

    Snapshot snapshot{};
    size_t offset = 0;

    while (offset + 10 <= data.size()) {
        // Mínimo 10 bytes para gameId(4) + players(4) + nameLen(2)
        GameInfo game{};
        game.id = readUInt32(data, offset);
        game.players = readUInt32(data, offset);

        // leer tamaño del nombre (2 bytes)
        if (offset + 2 > data.size()) break; // seguridad
        uint16_t nameLenBE;
        std::memcpy(&nameLenBE, &data[offset], sizeof(nameLenBE));
        offset += 2;
        uint16_t nameLen = ntohs(nameLenBE);

        // leer el nombre
        if (offset + nameLen > data.size()) break; // seguridad
        game.name = std::string(reinterpret_cast<char*>(&data[offset]), nameLen);
        offset += nameLen;

        snapshot.gameList.push_back(game);

        std::cout << "Debug: ID: " << game.id
                  << ", Jugadores: " << game.players
                  << " / " << Constants::MAX_PLAYERS_IN_GAME
                  << ", Creador: " << game.name << std::endl;
    }

    return snapshot;
}

std::optional<Snapshot> ClientProtocol::receiveMessageFromServer() {
    if (socket.is_stream_recv_closed()) return std::nullopt;

    uint8_t type;
    ssize_t bytes = socket.recvall(&type, sizeof(type));
    if (bytes <= 0) return std::nullopt;

    if (type == Constants::TYPE_SNAPSHOT) {
        return receiveSnapshotFromServer();
    }

    if (type == Constants::TYPE_CONTROL) {
        return receiveControlFromServer();
    }

    if (type == Constants::TYPE_GAME_LIST) {
        return receiveGameListFromServer();
    }

    std::cerr << "Mensaje desconocido recibido del servidor. Tipo = " << std::hex << (int)type << std::endl;
    return std::nullopt;
}

void ClientProtocol::sendString(const std::string& str) {
    uint16_t len = htons(str.size());
    socket.sendall(&len, sizeof(len));
    if (!str.empty()) {
        socket.sendall(str.data(), str.size());
    }
}
