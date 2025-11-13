#ifndef COMMAND_MESSAGE_H
#define COMMAND_MESSAGE_H


#include <SDL.h>

struct commandMessage
{
    SDL_KeyCode key;
    bool isPressed;
};
#endif // COMMAND_MESSAGE_H
