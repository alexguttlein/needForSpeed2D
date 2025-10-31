#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include "car.h"

#pragma pack(push, 1)  // para evitar padding
struct Message {
    uint8_t code = 0;
    char key    = '\0';
    int value   = 0;

    // constructor por defecto
    Message() = default;

    // constructor con parámetros
    Message(uint8_t c, char k) : code(c), key(k) {}
};
#pragma pack(pop)

#endif  // MESSAGE_H