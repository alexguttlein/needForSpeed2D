#include "client_receiverThread.h"

ReceiverThread::ReceiverThread(ClientProtocol& protocol, Queue<Snapshot>& queue, Queue<Event>& eventQueue)
    : protocol(protocol), snapshotQueue(queue), eventQueue(eventQueue), keepRunning(true) {}

void ReceiverThread::run() {
    try {
        while (keepRunning) {
            std::optional<Snapshot> optSnapshot = protocol.receiveMessageFromServer();
            if (!optSnapshot.has_value()) continue;

            Snapshot snapshot = optSnapshot.value();

            switch (snapshot.controlEvent) {
                case EventType::CREATE_JOIN_ACCEPTED: {
                    Event event = Event(EventType::CREATE_JOIN_ACCEPTED,
                    std::to_string(snapshot.playerId));
                    event.auxMessage = std::to_string(snapshot.gameId);
                    eventQueue.push(event);
                    continue;
                }
                case EventType::JOIN_REJECTED: {
                    eventQueue.push(Event(EventType::JOIN_REJECTED,
                    "No se pudo unir a la partida."));
                    continue;
                }
                case EventType::GAME_START: {
                    eventQueue.push(Event(EventType::GAME_START,
                    "Iniciando partida..."));
                    continue;
                }
                case EventType::SERVER_DISCONNECTED: {
                    std::cout << "debug: server disconnected detected" << std::endl;
                    eventQueue.push(Event(EventType::SERVER_DISCONNECTED,
                    "El servidor se ha desconectado."));
                    keepRunning = false; // se termina el loop del receiver
                    continue;
                }
                default: snapshotQueue.push(snapshot);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ReceiverThread] Excepción: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[ReceiverThread] Excepción desconocida.\n";
    }
    keepRunning = false;
}
