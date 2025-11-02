#include "server_protocol.h"

#include "common/constants.h"

ServerProtocol::ServerProtocol(Socket socket) :
    socket(std::move(socket)) , isClosed(false) {}

void ServerProtocol::closeSocket() {
    if (isClosed) return;
    socket.shutdown(SHUT_RDWR);
    socket.close();
}

bool ServerProtocol::isConnectionClosed() const {
    return socket.is_stream_send_closed() || socket.is_stream_recv_closed();
}

Message ServerProtocol::receiveMessage() {
    Message message;
    uint8_t msg(0);

    if (!isConnectionClosed()) {
        socket.recvall(&msg, sizeof(msg));
    }

    message.code = msg;
    message.key = commandToKey(msg);

    if (message.code == Constants::JOIN_GAME) {
        uint16_t matchIdBE;
        socket.recvall(&matchIdBE, sizeof(matchIdBE));

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

void ServerProtocol::sendSnapshot(const Snapshot& snapshot) {
    if (isConnectionClosed()) return;

    //se envia el tipo snapshot
    uint8_t type = Constants::TYPE_SNAPSHOT;
    socket.sendall(&type, sizeof(type));

    std::vector<uint8_t> buffer;

    //se envian las coordenadas
    appendUInt32(buffer, snapshot.posX);
    appendUInt32(buffer, snapshot.posY);

    socket.sendall(buffer.data(), buffer.size());
}

void ServerProtocol::sendControl(uint8_t code) {
    if (isConnectionClosed()) return;
    std::vector<uint8_t> buffer;

    buffer.push_back(Constants::TYPE_CONTROL); //se agrega el tipo control
    buffer.push_back(code); //se agrega el codigo de control
    socket.sendall(buffer.data(), buffer.size());
}

