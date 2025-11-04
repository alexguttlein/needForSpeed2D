#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "../common/constants.h"
#include "../common/queue.h"
#include "client_protocol.h"
#include "client_receiverThread.h"
#include "client_senderThread.h"
#include "../cmake-build-debug/_deps/sdl2-build/include/SDL2/SDL_keycode.h"
#include "../common/eventType.h"

class Client {
public:
    Client(const char* host, const char* port);
    void run();
private:
    ClientProtocol protocol;
    Queue<Snapshot> snapshotQueue;
    Queue<SDL_KeyCode> commandQueue;
    Queue<Event> eventQueue;
    ReceiverThread receiver;
    SenderThread sender;
    bool playing;
    void lobbyOptions();
};

#endif //CLIENT_H
