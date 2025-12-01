#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "../common/constants.h"
#include "../common/queue.h"
#include "client_protocol.h"
#include "client_receiverThread.h"
#include "client_senderThread.h"
#include "client_dibujador.h"
#include "client_audioManager.h"
#include <SDL.h>
#include "../common/eventType.h"
#include "commandMessage.h"
#include <memory>
#include <unordered_map>
#include <chrono>

class Client {
public:
    Client(const char* host, const char* port);
    void run();
    Queue<Event>& getEventQueue();
    ClientProtocol& getProtocol();
    Queue<Snapshot>& getSnapshotQueue();
    void setSelfId(int id);
    int getSelfId() const;
    void setSelectedCar(int id) { selectedCarId = id; }
    int getSelectedCar() const { return selectedCarId; }
    void setPlayerName(const std::string& name) { playerName = name; }
    const std::string& getPlayerName() const { return playerName; }
    bool sendLobbyOption(const std::string& option, const std::string& playerName, const int& carId);
    void changePlayingStatus();
    void setGameId(int gameId);
    int getGameId();

private:
    ClientProtocol protocol;
    Queue<Snapshot> snapshotQueue;
    Queue<commandMessage> commandQueue;
    Queue<Event> eventQueue;
    ReceiverThread receiver;
    SenderThread sender;
    bool playing;
    std::atomic<int> selfId{-1};
    int selectedCarId = -1;
    std::string playerName;
    std::unique_ptr<AudioManager> audioManager_;
    int accelerationChannel_ = -1;
        int gameId = 0;

    void loadTexturesAndAssets_(ClientDibujador& dib);
    void initAudio_();
};

#endif //CLIENT_H
