target_sources(taller_server
    PRIVATE
    # .cpp files
    server_main.cpp
    server_class.cpp
    server_acceptor.cpp
    server_clientHandler.cpp
    server_protocol.cpp
    server_senderThread.cpp
    server_receiverThread.cpp
    server_monitorClients.cpp
    server_snapshots.cpp
    PUBLIC
    # .h files
    server_class.h
    server_acceptor.h
    server_clientHandler.h
    server_protocol.h
    server_senderThread.h
    server_receiverThread.h
    server_monitorClients.h
    server_snapshots.h
    )
