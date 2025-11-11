#ifndef SERVER_ACCEPTOR_H
#define SERVER_ACCEPTOR_H

#include "../common/socket.h"
#include "../common/thread.h"
#include "server_clientHandler.h"
#include "server/server_gameMonitor.h"

#include <list>
#include <arpa/inet.h>
#include <memory>

class Acceptor : public Thread {

    private:
    Socket socket;
    bool keepAccepting;
    std::list<ClientHandler*> clients;
    GameMonitor gameMonitor;
    void closeSocket();
    void killDeadClients();
    void killClient(ClientHandler* client);

    public:
    /*
    * Constructor de Acceptor
    *
    * */
    Acceptor(const char* port);
    
    /*
    * Inicializa el Acceptor
    *
    * */
    void run() override;
    
    /*
    * Detiene la aceptación de nuevos clientes
    *
    * */
    void endAccepting();

    /*
    * Destructor de Acceptor
    *
    * */
    ~Acceptor() override;
};
#endif //SERVER_ACCEPTOR_H
