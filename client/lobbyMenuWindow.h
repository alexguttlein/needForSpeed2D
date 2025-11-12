#ifndef LOBBYMENUWINDOW_H
#define LOBBYMENUWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QMessageBox>
#include "waitingWindow.h"
#include <QtConcurrent/QtConcurrent>
#include "gameListWindow.h"

class Client;

class LobbyMenuWindow : public QWidget {
    Q_OBJECT
public:
    // explicit LobbyMenuWindow(const QString &playerName, QWidget *parent = nullptr);
    LobbyMenuWindow(Client* client, const QString& playerName, QWidget *parent = nullptr);

    QPushButton* getCreateButton();
    QPushButton* getJoinButton();
    QPushButton* getSelectCarButton();
    QString getPlayerName() const;
    void onCreateGameClicked();
    void onJoinGameClicked();

private:
    Client* client;
    QString playerName;
    QPushButton *createButton;
    QPushButton *joinButton;
    QPushButton *selectCarButton;
};

#endif //LOBBYMENUWINDOW_H