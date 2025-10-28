#ifndef SERVER_CLASS_H
#define SERVER_CLASS_H

#include "../common/socket.h"
#include "../common/queue.h"
#include "server_acceptor.h"
#include "server_gameloop.h"
#include "server_monitorClients.h"

#include <iostream>
#include <ostream>
#include <unistd.h>
#include <string>

class Server {

private:
    Queue<std::string> commandQueue;
    MonitorClients monitor;
    Acceptor acceptor;
    GameLoop gameLoop;

    void closeAcceptor();
    void closeGameLoop();
    void closeClients();


public:
    
    /*
    * Constructor de Server
    *
    * */
    explicit Server(const char* port);
    
    /*
    * Inicia el servidor
    *
    * */
    void run();

    /*
    * Destructor de Server
    *
    * */
    ~Server();
};
#endif //SERVER_CLASS_H
