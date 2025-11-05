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

void ClientProtocol::sendKey(const SDL_KeyCode input) {
    try {
        auto key = sdlToKey(input);
        uint8_t msg = CommandConstants::keyToBit(key);
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

uint32_t ClientProtocol::readBigEndianUInt32(const std::vector<uint8_t>& buffer,
        size_t& offset) {

    uint32_t value = 0;
    value |= static_cast<uint32_t>(buffer[offset])     << 24;
    value |= static_cast<uint32_t>(buffer[offset + 1]) << 16;
    value |= static_cast<uint32_t>(buffer[offset + 2]) << 8;
    value |= static_cast<uint32_t>(buffer[offset + 3]);
    offset += 4;
    return value;
}

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

        // leer posX
        uint32_t posXBE = 0;
        socket.recvall(&posXBE, sizeof(posXBE));
        snapshot.posX = ntohl(posXBE);

        // leer posY
        uint32_t posYBE = 0;
        socket.recvall(&posYBE, sizeof(posYBE));
        snapshot.posY = ntohl(posYBE);

        return snapshot;
    }

    if (type == Constants::TYPE_CONTROL) {
        // se recibio un codigo de control
        uint8_t code;
        socket.recvall(&code, sizeof(code));

        if (code == Constants::CREATE_JOIN_ACCEPTED) {
            Snapshot snapshot{};
            snapshot.controlEvent = EventType::CREATE_JOIN_ACCEPTED;
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
