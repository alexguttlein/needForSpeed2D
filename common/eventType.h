#ifndef EVENTTYPE_H
#define EVENTTYPE_H

#include <string>

enum class EventType {
    NONE,
    JOIN_REJECTED,
    CREATE_JOIN_ACCEPTED,
    GAME_LIST_RECEIVED
};

struct Event {
    EventType type;
    std::string message;

    explicit Event(EventType t = EventType::NONE, std::string msg = "")
        : type(t), message(std::move(msg)) {}
};

#endif //EVENTTYPE_H
