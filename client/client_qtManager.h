#ifndef CLIENT_QTMANAGER_H
#define CLIENT_QTMANAGER_H

#include <QApplication>
#include <QString>
#include <memory>
#include "lobbyMenuWindow.h"
#include "waitingWindow.h"

class Client;

/**
 * @brief Clase que administra toda la interfaz Qt del cliente.
 *
 * Encapsula las ventanas iniciales, la lógica de navegación
 * entre pantallas y las conexiones con el Client real (red y juego SDL).
 */
class ClientQtManager {
public:
    explicit ClientQtManager(Client* client);
    ~ClientQtManager();
    void start();

private:
    Client* client;
    std::unique_ptr<QApplication> app;

    void showLoginWindow();
    void showLobbyWindow(const QString& playerName);
    void setupCreateButton(LobbyMenuWindow* lobby);
    void setupJoinButton(LobbyMenuWindow* lobby);
    void setupSelectCarButton(LobbyMenuWindow* lobby);
};

#endif //CLIENT_QTMANAGER_H
