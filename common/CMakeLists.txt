target_sources(taller_common
    PRIVATE
    # .cpp files
    liberror.cpp
    resolver.cpp
    resolvererror.cpp
    socket.cpp
    commandConstants.cpp
    PUBLIC
    # .h files
    liberror.h
    resolver.h
    resolvererror.h
    socket.h
    thread.h
    queue.h
    constants.h
    message.h
    commandConstants.h
    snapshot.h
    carStateDTO.h
    raceStateDTO.h
    eventType.h
    gameInfo.h
    RaceCircuitDTO.h
    )
