#ifndef SERVER_GAMELOOP_H
#define SERVER_GAMELOOP_H

#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>
#include <memory>

#include "server_monitorClients.h"
#include "server_snapshots.h"
#include "../common/car.h"
#include "../common/thread.h"
#include "../common/constants.h"

class GameLoop : public Thread {

private:
    std::atomic<bool> running;
    MonitorClients& clients;
    Queue<std::string>& commandQueue;

    void run() override;

public:

    /*
    * Constructor de GameLoop
    *
    * */
    explicit GameLoop(Queue<std::string>& commandQueue, MonitorClients& clients);

    /*
    * Envía snapshots a todos los clientes conectados
    *
    * */
    void broadcastSnapshots();
    
    /*
    * Procesa la cola de comandos recibidos
    *
    * */
    void processCommandQueue();

    /*
    * Detiene el GameLoop
    *
    * */
    void stop() override;
    
    /*
    * Destructor de GameLoop
    *
    * */
    ~GameLoop();
};
#endif // SERVER_GAMELOOP_H
