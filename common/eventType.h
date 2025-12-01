#ifndef EVENTTYPE_H
#define EVENTTYPE_H

#include <string>

enum class EventType {
    NONE,
    JOIN_REJECTED,
    CREATE_JOIN_ACCEPTED,
    GAME_LIST_RECEIVED,
    COLLISION_CAR,
    COLLISION_BUILDING,
    GAME_START,
    SERVER_DISCONNECTED,
    PLAYER_COUNT_UPDATE
};

struct Event {
    EventType type;
    std::string message;
    std::string auxMessage = "";

    explicit Event(EventType t = EventType::NONE, std::string msg = "")
        : type(t), message(std::move(msg)) {}
};

#endif //EVENTTYPE_H
