target_sources(taller_client
    PRIVATE
    # .cpp files
    client_main.cpp
    client_class.cpp
    client_protocol.cpp
    client_receiverThread.cpp
    client_dibujador.cpp
    client_senderThread.cpp
    lobbyMenuWindow.cpp
    waitingWindow.cpp
    gameListWindow.cpp
    PUBLIC
    # .h files
    client_class.h
    client_protocol.h
    client_receiverThread.h
    client_dibujador.h
    client_senderThread.h
    lobbyMenuWindow.h
    waitingWindow.h
    gameListWindow.h
    )
