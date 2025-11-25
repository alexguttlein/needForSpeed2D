    #ifndef CLIENTHANDLER_H
    #define CLIENTHANDLER_H

    #include "common/socket.h"
    #include "common/queue.h"
    #include "common/constants.h"
    #include "common/message.h"
    #include "server/server_protocol.h"
    #include "server/server_senderThread.h"
    #include "common/snapshot.h"
    #include <algorithm>
    #include <atomic>

    class ReceiverThread;
    class GameMonitor;

    class ClientHandler {
    private:
        ServerProtocol protocol;
        GameMonitor& gameMonitor;
        Queue<std::shared_ptr<Snapshot>> clientQueue;
        Queue<std::shared_ptr<Message>>* sharedQueue;
        SenderThread senderThread;
        std::unique_ptr<ReceiverThread> receiverThread;
        int id;
        std::atomic<bool> alive{true};
        Snapshot snapshot;
        int currentGameId;
        std::atomic<bool> shuttingDown{false};
        std::string playerName;
        int carId;
        bool isPlaying;

    public:
        /*
        * Constructor de ClientHandler
        *
        * */
        explicit ClientHandler(Socket socket, int id, GameMonitor& gameMonitor);
        ~ClientHandler();

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
        void assignGameQueue(Queue<std::shared_ptr<Message>>& queue, int gameId);
        int getId() const;
        int getCurrentGameId() const;
        Queue<std::shared_ptr<Snapshot>>& getClientQueue();
        void setPlayerName(const std::string& string);
        void setCarId(int carId);
        void startGame();
    };
    #endif //CLIENTHANDLER_H
