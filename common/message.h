#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <string>

#pragma pack(push, 1)  // para evitar padding
struct Message {
    uint8_t code = 0;
    char key    = '\0';
    int intValue   = 0;
    std::string stringValue = std::string();
    int senderId = -1;

    // constructor por defecto
    Message() = default;

    // constructor con parámetros
    Message(uint8_t c, char k, int sender = -1)
        : code(c), key(k), senderId(sender) {}
};
#pragma pack(pop)

#endif  // MESSAGE_H