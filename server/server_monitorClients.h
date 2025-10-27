#ifndef SERVER_MONITORCLIENTS_H
#define SERVER_MONITORCLIENTS_H

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <utility>
#include "server_clientHandler.h"
#include "../common/message.h"

class MonitorClients {

private:
std::unordered_map<int, ClientHandler> clients;
mutable std::mutex mtx;


public:

/*
* Constructor de MonitorClients
*
* */
MonitorClients() = default;

/*
* Inserta un cliente en el monitor
*
* */
void insertClient(int id, ClientHandler&& client);

/*
* Elimina un cliente del monitor
*
* */
void deleteClient(int id);

/*
* Limpia todos los clientes del monitor
*
* */
void clear();

/*
* Obtiene un cliente del monitor
*
* */
ClientHandler& getClient(int id);

/*
* Aplica una función a todos los clientes del monitor
*
* */
void forEachClient(const std::function<void(ClientHandler&)>& func);

/*
* Aplica una función a un cliente específico del monitor
*
* */
void forClient(int id, const std::function<void(ClientHandler&)>& func);

/*
* Envía un mensaje a todos los clientes conectados
*
* */
void broadcastToAllClients(const Message& msg);

/*
* Elimina los clientes desconectados del monitor
*
* */
void killDisconnectedClients();

};
#endif  // SERVER_MONITORCLIENTS_H
