#ifndef TP_TALLER_G7_CLIENT_SENDERTHREAD_H
#define TP_TALLER_G7_CLIENT_SENDERTHREAD_H

#include "../common/thread.h"
#include "../common/queue.h"
#include "client_protocol.h"
#include <SDL.h>

#include "../cmake-build-debug/_deps/sdl2-build/include/SDL2/SDL_keycode.h"

class SenderThread : public Thread{
private:
    ClientProtocol& protocol;
    Queue<SDL_KeyCode>& commandQueue;

public:
    SenderThread(ClientProtocol& pro, Queue<SDL_KeyCode>& cmd_q);
    void run() override;
};


#endif //TP_TALLER_G7_CLIENT_SENDERTHREAD_H