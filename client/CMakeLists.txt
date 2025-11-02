target_sources(taller_client
    PRIVATE
    # .cpp files
    client_main.cpp
    client_class.cpp
    client_protocol.cpp
    client_receiverThread.cpp
    PUBLIC
    # .h files
    client_class.h
    client_protocol.h
    client_receiverThread.h
    )
