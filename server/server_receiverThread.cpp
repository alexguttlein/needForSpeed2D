#include "server_receiverThread.h"

ReceiverThread::ReceiverThread(ServerProtocol& protocol,
    Queue<std::shared_ptr<uint8_t>>& client_queue) :
    protocol(protocol), client_queue(client_queue) {
}

void ReceiverThread::run() {
    std::cout << "ReceiverThread::run()" << std::endl;
}
