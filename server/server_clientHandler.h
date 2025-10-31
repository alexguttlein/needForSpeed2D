#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "common/socket.h"
#include "common/queue.h"
#include "common/constants.h"
#include "common/message.h"
#include "server/server_protocol.h"
#include "server/server_senderThread.h"
#include "server/server_receiverThread.h"
#include "common/snapshot.h"
#include "server/server_gameMonitor.h"

#include <algorithm>

class ClientHandler {
private:
    ServerProtocol protocol;
    GameMonitor& gameMonitor;
    Queue<Snapshot> clientQueue;
    Queue<std::shared_ptr<Message>>* sharedQueue;
    SenderThread senderThread;
    ReceiverThread receiverThread;
    int id;
    bool alive;
    Snapshot snapshot;

public:

    /*
    * Constructor de ClientHandler
    *
    * */
    explicit ClientHandler(Socket socket, int id, GameMonitor& gameMonitor);

    /*
    * Inicia los threads de envío y recepción de mensajes
    *
    * */
    void startThreads();

    /*
    * Cierra los threads y la conexión con el cliente
    *
    * */
    void shutdown();
    
    /*
    * Indica si el cliente sigue conectado
    *
    * */
    bool isConnected() const;
    bool isAlive() const;
    void killClient();
    void assignGameQueue(Queue<std::shared_ptr<Message>>& queue);
    int getId() const;
};
#endif //CLIENTHANDLER_H
