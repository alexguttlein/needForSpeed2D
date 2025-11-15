#ifndef LOBBYMENUWINDOW_H
#define LOBBYMENUWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSignalMapper>
#include <QtConcurrent/QtConcurrent>
#include "gameListWindow.h"
#include "waitingWindow.h"

class Client;

class LobbyMenuWindow : public QWidget {
    Q_OBJECT
public:
    LobbyMenuWindow(Client* client, const QString& playerName, QWidget *parent = nullptr);

    QPushButton* getCreateButton();
    QPushButton* getJoinButton();
    QPushButton* getSelectCarButton();
    QString getPlayerName() const;
    bool carChosen = false;

    signals:
        void carSelected(int carId); // señal para avisar que se eligió auto

private:
    Client* client;
    QString playerName;
    QPushButton *createButton;
    QPushButton *joinButton;
    QPushButton *selectCarButton;
};

#endif //LOBBYMENUWINDOW_H
