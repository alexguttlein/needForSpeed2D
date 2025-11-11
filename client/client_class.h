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
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QObject>
#include <QLabel>
#include <QPixmap>
#include <QPalette>
#include <QSpacerItem>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QGraphicsColorizeEffect>
#include "client/lobbyMenuWindow.h"

class Client {
public:
    Client(const char* host, const char* port);
    void init();
    void startGame();
    void setPlayerName(const std::string& name);
    void run();
private:
    ClientProtocol protocol;
    Queue<Snapshot> snapshotQueue;
    Queue<SDL_KeyCode> commandQueue;
    Queue<Event> eventQueue;
    ReceiverThread receiver;
    SenderThread sender;
    bool playing;
    std::string playerName;
    void goToLobby();
    void lobbyOptions();
};

#endif //CLIENT_H
