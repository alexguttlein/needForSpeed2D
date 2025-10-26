#include "server_senderThread.h"

SenderThread::SenderThread(ServerProtocol& protocol,
    Queue<std::shared_ptr<uint8_t>>& client_queue) :
    protocol(protocol), client_queue(client_queue) {
}

void SenderThread::run() {
    std::cout << "SenderThread::run()" << std::endl;
}
