#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include "common/socket.h"
#include "common/message.h"
#include "common/commandConstants.h"
#include "common/snapshot.h"
#include <arpa/inet.h>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <vector>
#include <memory>

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

    void sendSnapshot(std::shared_ptr<Snapshot>& snapshot);
    void sendControl(uint8_t code);
    void sendGamesList(uint8_t& type, const std::vector<unsigned char>& buffer);
    void sendCreateJoinAccepted(std::vector<uint8_t>& buffer);
    void sendGameStart(const std::vector<uint8_t>& buffer);

private:
    Socket socket;
    bool isClosed;
    char commandToKey(const uint8_t& command);
    std::string receiveString();
};

#endif //SERVER_PROTOCOL_H