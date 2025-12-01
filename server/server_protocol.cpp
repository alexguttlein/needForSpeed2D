#include "server_protocol.h"
#include "common/constants.h"
#include "common/eventType.h"

ServerProtocol::ServerProtocol(Socket socket) :
    socket(std::move(socket)) , isClosed(false) {}

void ServerProtocol::closeSocket() {
    if (isConnectionClosed()) return;
    socket.shutdown(SHUT_RDWR);
    socket.close();
}

bool ServerProtocol::isConnectionClosed() const {
    return socket.is_stream_send_closed() || socket.is_stream_recv_closed();
}

Message ServerProtocol::receiveMessage() {
    Message message;
    uint8_t msg(0);

    if (isConnectionClosed()) return message;

    if (!isConnectionClosed()) {
        int ret = socket.recvall(&msg, sizeof(msg));
        if (ret <= 0) { // socket cerrado o error
            throw std::runtime_error("Connection closed");
        }
    }

    message.code = msg;
    message.key = commandToKey(msg);

    if (message.code == Constants::JOIN_GAME) {
        uint16_t matchIdBE;
        int ret = socket.recvall(&matchIdBE, sizeof(matchIdBE));
        if (ret <= 0) { // socket cerrado o error
            throw std::runtime_error("Connection closed");
        }

        uint16_t matchId = ntohs(matchIdBE);
        message.intValue = static_cast<int>(matchId);
        std::cout << "Debug: Cliente quiere unirse a la partida con ID: " << matchId << std::endl;
        message.stringValue = receiveString(); //se recibe el nombre del player

        // se recibe id del auto elegido
        uint32_t carIdBE;
        socket.recvall(&carIdBE, sizeof(carIdBE));
        message.carId = static_cast<int>(ntohl(carIdBE));

    } else if (message.code == Constants::CREATE_GAME) {
        message.stringValue = receiveString(); //se recibe el nombre del player
        // se recibe id del auto elegido
        uint32_t carIdBE;
        socket.recvall(&carIdBE, sizeof(carIdBE));
        message.carId = static_cast<int>(ntohl(carIdBE));

    } else if (message.code == Constants::GAME_START) {
        uint32_t gameIdBE;
        socket.recvall(&gameIdBE, sizeof(gameIdBE));
        message.intValue = static_cast<int>(ntohl(gameIdBE)); //se carga el gameId en el mensaje
    }
    return message;
}

std::string ServerProtocol::receiveString() {
    Message message{};

    uint16_t nameSize_net;
    socket.recvall(&nameSize_net, sizeof(nameSize_net));
    uint16_t nameSize = ntohs(nameSize_net);

    if (nameSize == 0) {message.stringValue = std::string();}
    std::string result(nameSize, '\0');
    socket.recvall(result.data(), nameSize);
    return result;
}

void ServerProtocol::appendUInt32(std::vector<uint8_t>& buffer, uint32_t value) {
    uint32_t be_value = htonl(value);  // Host → Network (big endian)
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&be_value);
    buffer.insert(buffer.end(), bytes, bytes + sizeof(be_value));
}

char ServerProtocol::commandToKey(const uint8_t& command) {
    const uint8_t KEY_BITS = command & 0x7F; // Mask to get the lower 7 bits
    try {
        CommandConstants::Key key = CommandConstants::bitToKey(
            static_cast<CommandConstants::Bit>(KEY_BITS)
        );
        return static_cast<char>(key);
    } catch (const std::invalid_argument& e) {
        // Byte recibido no corresponde a ninguna tecla
        return '\0';
    }
}

void ServerProtocol::addIntToUint8tVector(std::vector<uint8_t>& buffer, int value) {
    uint32_t num = static_cast<uint32_t>(value);
    appendBigEndian(buffer, num);
}

template<typename T>
void ServerProtocol::appendBigEndian(std::vector<uint8_t>& buffer, T value) {
    for (int i = sizeof(T) - 1; i >= 0; --i) {
        buffer.push_back(static_cast<uint8_t>((value >> (8 * i)) & 0xFF));
    }
}

void ServerProtocol::sendSnapshot(std::shared_ptr<Snapshot>& snapshot) {
    if (isConnectionClosed()) return;

    //se envia el tipo snapshot
    uint8_t type = Constants::TYPE_SNAPSHOT;
    socket.sendall(&type, sizeof(type));

    std::vector<uint8_t> buffer;

    // controlEvent primero (1 byte)
    buffer.push_back(static_cast<uint8_t>(snapshot->controlEvent));

    //player id
    addIntToUint8tVector(buffer, snapshot->playerId);

    //players size
    appendUInt32(buffer, snapshot->playersSize);

    for (const auto& carState : snapshot->cars) {
        
        // car_id (int, típicamente 4 bytes)
        addIntToUint8tVector(buffer, carState.car_id);

        // car_type_id (int, típicamente 4 bytes)
        addIntToUint8tVector(buffer, carState.car_type_id);

        // currentUpgradeId (int, típicamente 4 bytes)
        addIntToUint8tVector(buffer, carState.currentUpgradeId);
        
        // health (float, 4 bytes)
        appendUInt32(buffer, *reinterpret_cast<const uint32_t*>(&carState.health));

        // speed (float, 4 bytes)
        appendUInt32(buffer, *reinterpret_cast<const uint32_t*>(&carState.speed));
        
        // position (Vector2D<float>, 8 bytes total)
        appendUInt32(buffer, *reinterpret_cast<const uint32_t*>(&carState.position.x));
        appendUInt32(buffer, *reinterpret_cast<const uint32_t*>(&carState.position.y));

        // angle (Vector2D<float>, 8 bytes total)
        // Nota: asumo que angle guarda el coseno y seno (c, s) del ángulo como Vector2D.
        appendUInt32(buffer, *reinterpret_cast<const uint32_t*>(&carState.angle.x)); // cos(angle)
        appendUInt32(buffer, *reinterpret_cast<const uint32_t*>(&carState.angle.y)); // sin(angle)
    }

    // raceStates: uno por jugador, alineado con cars
    uint32_t raceCount = static_cast<uint32_t>(snapshot->raceStates.size());
    appendUInt32(buffer, raceCount);

    for (const auto& rs : snapshot->raceStates) {
        
        // playerName (string: uint16_t length + chars)
        uint16_t nameLen = static_cast<uint16_t>(rs.playerName.size());
        uint16_t nameLenBE = htons(nameLen);
        const uint8_t* nameLenBytes = reinterpret_cast<const uint8_t*>(&nameLenBE);
        buffer.insert(buffer.end(), nameLenBytes, nameLenBytes + sizeof(nameLenBE));
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(rs.playerName.data()), reinterpret_cast<const uint8_t*>(rs.playerName.data()) + nameLen);
        
        // playerId
        addIntToUint8tVector(buffer, rs.playerId);

        
        // timeLeftRace (string: uint16_t length + chars)
        uint16_t timeLen = static_cast<uint16_t>(rs.timeLeftRace.size());
        uint16_t timeLenBE = htons(timeLen);
        const uint8_t* timeLenBytes = reinterpret_cast<const uint8_t*>(&timeLenBE);
        buffer.insert(buffer.end(), timeLenBytes, timeLenBytes + sizeof(timeLenBE));
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(rs.timeLeftRace.data()), reinterpret_cast<const uint8_t*>(rs.timeLeftRace.data()) + timeLen);
        
        // currentRaceId
        addIntToUint8tVector(buffer, rs.currentRaceId);

        // checkpointsSize
        addIntToUint8tVector(buffer, rs.checkpointsSize);

        // nextCheckpoint (Vector2D<float>, 8 bytes total)
        appendUInt32(buffer, *reinterpret_cast<const uint32_t*>(&rs.nextCheckpoint.x));
        appendUInt32(buffer, *reinterpret_cast<const uint32_t*>(&rs.nextCheckpoint.y));

        // currentHints (uint32_t size + Vector2D<float> * size)
        uint32_t hintsSize = static_cast<uint32_t>(rs.currentHints.size());
        appendUInt32(buffer, hintsSize);
        for (const auto& hint : rs.currentHints) {
            appendUInt32(buffer, *reinterpret_cast<const uint32_t*>(&hint.x));
            appendUInt32(buffer, *reinterpret_cast<const uint32_t*>(&hint.y));
        }

        // hasFinished (1 byte)
        buffer.push_back(static_cast<uint8_t>(rs.hasFinished));

        // finishPosition (int, típicamente 4 bytes)
        addIntToUint8tVector(buffer, rs.finishPosition);
    }

    // Flag de fin de carrera (1 byte)
    buffer.push_back(static_cast<uint8_t>(snapshot->raceFinished));

    // Tiempos de llegada (float por jugador, en el mismo orden que raceStates)
    for (const auto& rs : snapshot->raceStates) {
        uint32_t timeBits = *reinterpret_cast<const uint32_t*>(&rs.finishTimeSeconds);
        appendUInt32(buffer, timeBits);
    }

    // Flag de fin de juego (1 byte)
    buffer.push_back(static_cast<uint8_t>(snapshot->gameFinished));

   uint32_t leaderboardSize = static_cast<uint32_t>(snapshot->leaderboards.size());
   appendUInt32(buffer, leaderboardSize); 

    // 🟢 SOLO ENVIAR LOS DATOS DE LOS JUGADORES SI EL TAMAÑO ES > 0
    for (const auto& playerTime : snapshot->leaderboards) {
        // Si leaderboardSize es 0, este bucle no se ejecuta.
        addIntToUint8tVector(buffer, playerTime.playerId);
        uint32_t timeBits = *reinterpret_cast<const uint32_t*>(&playerTime.finishTime);
        appendUInt32(buffer, timeBits);
        // Enviar playerName
        uint16_t nameLen = static_cast<uint16_t>(playerTime.playerName.size());
        uint16_t nameLenBE = htons(nameLen);
        const uint8_t* nameLenBytes = reinterpret_cast<const uint8_t*>(&nameLenBE);
        buffer.insert(buffer.end(), nameLenBytes, nameLenBytes + sizeof(nameLenBE));
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(playerTime.playerName.data()), reinterpret_cast<const uint8_t*>(playerTime.playerName.data()) + nameLen);
    }

    // Enviar eventos de colisión
    uint32_t collisionCount = static_cast<uint32_t>(snapshot->collisions.size());
    appendUInt32(buffer, collisionCount);
    
    for (const auto& collision : snapshot->collisions) {
        addIntToUint8tVector(buffer, collision.playerId);
        buffer.push_back(static_cast<uint8_t>(collision.collisionType));
    }

    socket.sendall(buffer.data(), buffer.size());
}

void ServerProtocol::sendControl(uint8_t code) {
    if (isConnectionClosed()) return;
    std::vector<uint8_t> buffer;

    buffer.push_back(Constants::TYPE_CONTROL); //se agrega el tipo control
    buffer.push_back(code); //se agrega el codigo de control
    socket.sendall(buffer.data(), buffer.size());
}

void ServerProtocol::sendGamesList(uint8_t& type, const std::vector<unsigned char>& vector) {
    if (socket.is_stream_send_closed()) return;
    std::vector<uint8_t> buffer;
    buffer.push_back(type);

    uint32_t vecSizeBE = htonl(static_cast<uint32_t>(vector.size()));
    uint8_t* sizeBytes = reinterpret_cast<uint8_t*>(&vecSizeBE);

    buffer.insert(buffer.end(), sizeBytes, sizeBytes + sizeof(vecSizeBE));
    buffer.insert(buffer.end(), vector.begin(), vector.end());

    socket.sendall(buffer.data(), buffer.size());
}

void ServerProtocol::sendControl(std::vector<uint8_t>& buffer) {
    if (isConnectionClosed()) return;
    socket.sendall(buffer.data(), buffer.size());
}
