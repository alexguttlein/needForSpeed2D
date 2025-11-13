#ifndef TP_TALLER_G7_CLIENT_SENDERTHREAD_H
#define TP_TALLER_G7_CLIENT_SENDERTHREAD_H

#include "../common/thread.h"
#include "../common/queue.h"
#include "client_protocol.h"
#include "commandMessage.h"
#include <SDL.h>


class SenderThread : public Thread{
private:
    ClientProtocol& protocol;
    Queue<commandMessage>& commandQueue;

public:
    SenderThread(ClientProtocol& pro, Queue<commandMessage>& cmd_q);
    void run() override;
};


#endif //TP_TALLER_G7_CLIENT_SENDERTHREAD_H