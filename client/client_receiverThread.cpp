#include "client_receiverThread.h"

ReceiverThread::ReceiverThread(ClientProtocol& protocol, Queue<Snapshot>& queue)
    : protocol(protocol), snapshotQueue(queue), keepRunning(true) {}

void ReceiverThread::run() {
    try {
        while (keepRunning) {
            Snapshot snapshot = protocol.receiveSnapshot();
            std::cout << "debug: client recibio " << snapshot.posY << std::endl;
            snapshotQueue.push(snapshot);
        }
    } catch (const std::exception& e) {
        std::cerr << "[ReceiverThread] Excepción: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[ReceiverThread] Excepción desconocida.\n";
    }
    keepRunning = false;
}
