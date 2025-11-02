#include "client_receiverThread.h"

ReceiverThread::ReceiverThread(ClientProtocol& protocol, Queue<Snapshot>& queue, Queue<Event>& eventQueue)
    : protocol(protocol), snapshotQueue(queue), eventQueue(eventQueue), keepRunning(true) {}

void ReceiverThread::run() {
    try {
        while (keepRunning) {
            std::optional<Snapshot> optSnapshot = protocol.receiveMessageFromServer();
            if (!optSnapshot.has_value()) continue;

            Snapshot snapshot = optSnapshot.value();
            if (snapshot.controlEvent == EventType::CREATE_JOIN_ACCEPTED) {
                eventQueue.push(Event(EventType::CREATE_JOIN_ACCEPTED,
                    "Creación o unión a partida satisfactoria."));
                continue;
            } else if (snapshot.controlEvent == EventType::JOIN_REJECTED) {
                eventQueue.push(Event(EventType::JOIN_REJECTED,
                    "No se pudo unir a la partida."));
                continue;
            }

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
