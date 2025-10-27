#include "server_monitorClients.h"

void MonitorClients::insertClient(int id, ClientHandler&& client) {
    std::lock_guard<std::mutex> lock(mtx);
    clients.emplace(id, std::move(client));
}

void MonitorClients::deleteClient(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    clients.erase(id);
}

void MonitorClients::clear() {
    std::lock_guard<std::mutex> lock(mtx);
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

void MonitorClients::broadcastToAllClients(const Message& msg) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& [id, client] : clients) {
        client.sendMessage(msg);
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
