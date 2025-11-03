#ifndef SERVER_GAMELOOP_H
#define SERVER_GAMELOOP_H

#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>
#include <memory>


#include "server_snapshots.h"
#include "../common/thread.h"
#include "../common/constants.h"
#include "server_gamelogic.h"

class GameLoop : public Thread {

private:
    std::atomic<bool> running;
    Queue<std::string>& commandQueue;
    GameLogic gameLogic;
    WorldSnapshots snapshots;

public:

    /*
    * Constructor de GameLoop
    *
    * */
    explicit GameLoop(Queue<std::string>& commandQueue);

    /*
    * Agrega las snapshots a la queue de snapshots
    *
    * */
    void saveSnapshots();

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
    * Ejecuta el GameLoop
    *
    * */
    void run() override;
    
    /*
    * Destructor de GameLoop
    *
    * */
    ~GameLoop();
};
#endif // SERVER_GAMELOOP_H
