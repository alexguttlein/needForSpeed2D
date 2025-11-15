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
#include <SDL.h>
#include "../common/eventType.h"
#include "commandMessage.h"

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
};

#endif //CLIENT_H
