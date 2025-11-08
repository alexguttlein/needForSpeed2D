#ifndef SERVER_GAMELOOP_H
#define SERVER_GAMELOOP_H

#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>
#include <memory>

#include "common/snapshot.h"
#include "../common/thread.h"
#include "../common/constants.h"
#include "common/queue.h"
#include "common/message.h"
#include "server_gamelogic.h"
#include <unordered_map>

class GameLoop : public Thread {

private:
    GameLogic gameLogic;
    std::atomic<bool> running;
    Queue<std::shared_ptr<Message>>& commandQueue; //queue compartida
    std::vector<Queue<std::shared_ptr<Snapshot>>*> clientQueues; //queues privadas de los jugadores
    std::mutex qmtx;

public:

    /*
    * Constructor de GameLoop
    *
    * */
    // explicit GameLoop(Queue<std::string>& commandQueue);
    explicit GameLoop(Queue<std::shared_ptr<Message>>& commandQueue,
                      std::vector<Queue<std::shared_ptr<Snapshot>>*> clientQueues);

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

    /*
    * Agrega un nuevo jugador al GameLoop
    *
    * */
    void addPlayer(int playerId);
};
#endif // SERVER_GAMELOOP_H
