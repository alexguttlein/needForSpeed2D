#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>

#pragma pack(push, 1)  // para evitar padding
struct Player {
    int playerId;
    uint32_t posX;
    uint32_t posY;
};
#pragma pack(pop)

#endif //PLAYER_H