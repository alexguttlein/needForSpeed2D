#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "common/queue.h"
#include "common/message.h"
#include "common/constants.h"

class Game {
public:
    explicit Game(int gameId);
    Queue<std::shared_ptr<Message>>& getSharedQueue();
private:
    int gameId;
    Queue<std::shared_ptr<Message>> sharedQueue;
};

#endif //SERVER_GAME_H