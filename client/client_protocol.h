#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <arpa/inet.h>
#include <SDL.h>
#include <vector>
#include <cstring>
#include <sstream>
#include <optional>

#include "../common/constants.h"
#include "../common/commandConstants.h"
#include "../common/socket.h"
#include "../common/snapshot.h"
#include "../common/gameInfo.h"

class ClientProtocol {
public:
    ClientProtocol(const char* host, const char* port);
    ~ClientProtocol();
    void sendKey(const SDL_KeyCode msg);
    CommandConstants::Key sdlToKey(const SDL_KeyCode input);
    bool sendLobbyOption(const std::string& input);
    std::optional<Snapshot> receiveMessageFromServer();

private:
    Socket socket;
    bool isClosed;
    // uint32_t readBigEndianUInt32(const std::vector<uint8_t>& buffer, size_t& offset);
    uint32_t readUInt32(const std::vector<uint8_t>& buffer, size_t& offset);
};

#endif //CLIENT_PROTOCOL_H
