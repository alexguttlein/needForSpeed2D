#ifndef GAMELISTWINDOW_H
#define GAMELISTWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>
#include "client_class.h"

class Client;

class GameListWindow : public QWidget {
    Q_OBJECT
public:
    explicit GameListWindow(Client* client, QWidget* parent = nullptr);

    void loadGameList(const std::vector<GameInfo>& games);

    signals:
        void gameSelected(uint32_t gameId);

private:
    Client* client;
    QTableWidget* table;
    QPushButton* refreshButton;
    QPushButton* cancelButton;

private slots:
    void onRefreshClicked();
    void onCancelClicked();
    void onTableDoubleClicked(int row, int column);
};

#endif //GAMELISTWINDOW_H
