#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include "common/socket.h"
#include "common/message.h"
#include "common/commandConstants.h"
#include "server/server_snapshots.h"
#include <arpa/inet.h>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <iostream>

class ServerProtocol {
public:
    explicit ServerProtocol(Socket socket);
    bool isConnectionClosed() const;
    void closeSocket();
    Message receiveMessage();

    void addIntToUint8tVector(std::vector<uint8_t>& vector, int value);
    void appendUInt32(std::vector<uint8_t>& buffer, uint32_t value);

    template<typename T>
    void appendBigEndian(std::vector<uint8_t>& vec, T value);

    void sendSnapshot(const Snapshot& snapshot);
    void sendControl(uint8_t code);
    void sendGamesList(uint8_t& type, const std::vector<unsigned char>& buffer);

private:
    Socket socket;
    bool isClosed;
    char commandToKey(const uint8_t& command);
};

#endif //SERVER_PROTOCOL_H