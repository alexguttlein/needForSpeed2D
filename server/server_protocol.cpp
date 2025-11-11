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
    }

    return message;
}

void ServerProtocol::appendUInt32(std::vector<uint8_t>& buffer, uint32_t value) {
    uint32_t be_value = htonl(value);  // Host → Network (big endian)
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&be_value);
    buffer.insert(buffer.end(), bytes, bytes + sizeof(be_value));
}

char ServerProtocol::commandToKey(const uint8_t& command) {
    try {
        CommandConstants::Key key = CommandConstants::bitToKey(
            static_cast<CommandConstants::Bit>(command)
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

    for (uint32_t i = 0; i < snapshot->playersSize; ++i) {
        addIntToUint8tVector(buffer, snapshot->players.at(i).playerId);
        // posX y posY en big-endian
        appendUInt32(buffer, snapshot->players.at(i).posX);
        appendUInt32(buffer, snapshot->players.at(i).posY);

        std::cout << "debug: se va a enviar: " << snapshot->players.at(i).posX << ", " <<
            snapshot->players.at(i).posY << std::endl;
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

void ServerProtocol::sendCreateJoinAccepted(std::vector<uint8_t>& buffer) {
    if (isConnectionClosed()) return;
    socket.sendall(buffer.data(), buffer.size());
}