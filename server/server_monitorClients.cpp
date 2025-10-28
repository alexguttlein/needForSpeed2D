#include "server_monitorClients.h"


ClientHandler& MonitorClients::insertClient(int id, Socket socket) {
    std::lock_guard<std::mutex> lock(mtx);
    auto [it, inserted] = clients.try_emplace(id, std::move(socket));
    return it->second;
}


void MonitorClients::deleteClient(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    clients.erase(id);
}

void MonitorClients::clear() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& [id, client] : clients) {
        client.shutdown();
    }
    clients.clear();
}

ClientHandler& MonitorClients::getClient(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    return clients.at(id);
}

void MonitorClients::forEachClient(const std::function<void(ClientHandler&)>& func) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& [id, client] : clients) {
        func(client);
    }
}

void MonitorClients::forClient(int id, const std::function<void(ClientHandler&)>& func) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = clients.find(id);
    if (it != clients.end()) {
        func(it->second);
    }
}

void MonitorClients::broadcastToAllClients(const std::shared_ptr<Message>& msg) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& [id, client] : clients) {
        client.enqueueMessage(msg);
    }
}

void MonitorClients::killDisconnectedClients() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto it = clients.begin(); it != clients.end();) {
        if (!it->second.isConnected()) {
            it = clients.erase(it);
        } else {
            ++it;
        }
    }
}
