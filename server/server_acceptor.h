#ifndef SERVER_ACCEPTOR_H
#define SERVER_ACCEPTOR_H

#include "../common/socket.h"
#include "../common/thread.h"
#include "server_clientHandler.h"
#include "server_monitorClients.h"

#include <arpa/inet.h>
#include <memory>

class Acceptor : public Thread {

    private:
    Socket socket;
    MonitorClients& monitorClients;
    bool keepAccepting;
    void closeSocket();


    public:

    /*
    * Constructor de Acceptor
    *
    * */
    Acceptor(const char* port, MonitorClients& monitorClients);
    
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
    * Agrega un nuevo cliente al monitor de clientes
    *
    * */
   void addNewClient(int id, Socket newSocket);
    

};

#endif //SERVER_ACCEPTOR_H
