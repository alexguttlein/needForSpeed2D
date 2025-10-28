#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "common/socket.h"
#include "common/queue.h"
#include "common/constants.h"
#include "common/message.h"
#include "server/server_protocol.h"
#include "server/server_senderThread.h"
#include "server/server_receiverThread.h"
#include "server/server_snapshots.h"

#include <algorithm>

class ClientHandler {

private:
    ServerProtocol protocol;
    Queue<std::shared_ptr<Message>> clientQueue;
    Snapshots snapshots;
    SenderThread senderThread;
    ReceiverThread receiverThread;

public:

    /*
    * Constructor de ClientHandler
    *
    * */
    explicit ClientHandler(Socket socket);

    /*
    * Inicia los threads de envío y recepción de mensajes
    *
    * */
    void startThreads();

    /*
    * Encola un mensaje para ser enviado al cliente
    *
    * */
    void enqueueMessage(const std::shared_ptr<Message>& msg);
    
    /*
    * Indica si el cliente sigue conectado
    *
    * */
    bool isConnected() const;
    
    /*
    * Obtiene los snapshots del cliente
    *
    * */
    Snapshots& getSnapshots();

};
#endif //CLIENTHANDLER_H
