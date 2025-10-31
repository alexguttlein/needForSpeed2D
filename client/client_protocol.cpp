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

void ClientProtocol::send(const SDL_KeyCode input) {
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

Snapshot ClientProtocol::receiveSnapshot() {
    Snapshot snapshot{};
    std::vector<uint8_t> buffer(8);

    if (socket.is_stream_recv_closed()) return snapshot;

    socket.recvall(buffer.data(), buffer.size());

    size_t offset = 0;
    snapshot.posX = readUInt32(buffer, offset);
    snapshot.posY = readUInt32(buffer, offset);

    return snapshot;
}

void ClientProtocol::sendCreateGame() {
    try {
        uint8_t msg = 0x09;
        socket.sendall(&msg, sizeof(msg));
    } catch (const std::exception& e) {
        // tecla no reconocida, no enviamos nada
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
