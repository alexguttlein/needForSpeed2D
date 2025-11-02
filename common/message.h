#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include "../server/car.h"

#pragma pack(push, 1)  // para evitar padding
struct Message {
    uint8_t code; 
    Car car;
};
#pragma pack(pop)

#endif  // MESSAGE_H