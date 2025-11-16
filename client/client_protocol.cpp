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

// uint32_t ClientProtocol::readBigEndianUInt32(const std::vector<uint8_t>& buffer,
//         size_t& offset) {
//
//     uint32_t value = 0;
//     value |= static_cast<uint32_t>(buffer[offset])     << 24;
//     value |= static_cast<uint32_t>(buffer[offset + 1]) << 16;
//     value |= static_cast<uint32_t>(buffer[offset + 2]) << 8;
//     value |= static_cast<uint32_t>(buffer[offset + 3]);
//     offset += 4;
//     return value;
// }

bool ClientProtocol::sendLobbyOption(const std::string& input) {
    if (socket.is_stream_send_closed()) return false;
    std::istringstream iss(input);
    std::string command;
    iss >> command;

    if (command == "crear") {
        uint8_t msg = Constants::CREATE_GAME;
        socket.sendall(&msg, sizeof(msg));
        return true;
    } else if (command == "unirse") {
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
        return true;

    } else if (command == "listar") {
        uint8_t msg = Constants::LIST_GAMES;
        socket.sendall(&msg, sizeof(msg));
        return false; // sigue en el lobby
    }
    std::cerr << "Opción inválida. Usa 'crear' o 'unirse <id>'" << std::endl;
    return false;
}

std::optional<Snapshot> ClientProtocol::receiveMessageFromServer() {
    if (socket.is_stream_recv_closed()) return std::nullopt;

    uint8_t type;
    ssize_t bytes = socket.recvall(&type, sizeof(type));
    if (bytes <= 0) return std::nullopt;

    if (type == Constants::TYPE_SNAPSHOT) {
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
        //std::cout << "debug: Player id: " << snapshot.playerId << std::endl;

        for (uint32_t i = 0; i < snapshot.playersSize; i++) {
        CarStateDTO dto{};
        
        uint32_t idBE = 0;
        socket.recvall(&idBE, sizeof(idBE));
        dto.car_id = static_cast<int>(ntohl(idBE));

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

            // playerId
            uint32_t playerIdBE = 0;
            socket.recvall(&playerIdBE, sizeof(playerIdBE));
            rs.playerId = static_cast<int>(ntohl(playerIdBE));

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

        return snapshot;
    }

    if (type == Constants::TYPE_CONTROL) {
        // se recibio un codigo de control
        uint8_t code;
        socket.recvall(&code, sizeof(code));

        if (code == Constants::CREATE_JOIN_ACCEPTED) {
            //se recibe playerId
            uint32_t playerIdBE = 0;
            socket.recvall(&playerIdBE, sizeof(playerIdBE));
            int selfId = ntohl(playerIdBE);

            Snapshot snapshot{};
            snapshot.controlEvent = EventType::CREATE_JOIN_ACCEPTED;
            snapshot.playerId = selfId;
            return snapshot;
        } else if (code == Constants::JOIN_REJECTED) {
            Snapshot snapshot{};
            snapshot.controlEvent = EventType::JOIN_REJECTED;
            return snapshot;
        } else {
            std::cerr << "Código de control recibido: " << std::hex << (int)code << std::endl;
        }
        return std::nullopt;
    }

    if (type == Constants::TYPE_GAME_LIST) {
        // Leer tamaño de la lista (4 bytes)
        uint32_t sizeBE = 0;
        if (socket.recvall(&sizeBE, sizeof(sizeBE)) <= 0) return std::nullopt;
        uint32_t listSize = ntohl(sizeBE);

        // Leer la lista completa
        std::vector<uint8_t> data(listSize);
        if (socket.recvall(data.data(), listSize) <= 0) return std::nullopt;

        // se arma la lista
        std::cout << "Partidas activas:\n";
        size_t offset = 0;
        while (offset + 8 <= data.size()) { // 4 bytes id + 4 bytes jugadores
            uint32_t id = readUInt32(data, offset);
            uint32_t players = readUInt32(data, offset);
            std::cout << "ID: " << id <<
                ", Jugadores: " << players << " / " <<
                    Constants::MAX_PLAYERS_IN_GAME << std::endl;
        }
        return std::nullopt; // sigue en lobby
    }

    std::cerr << "Mensaje desconocido recibido del servidor. Tipo = " << std::hex << (int)type << std::endl;
    return std::nullopt;
}
