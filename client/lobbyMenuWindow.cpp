#include "lobbyMenuWindow.h"
#include "client_class.h"
#include "common/eventType.h"
#include "waitingWindow.h"
#include <QVBoxLayout>
#include <QPixmap>
#include <QPalette>

LobbyMenuWindow::LobbyMenuWindow(Client* client, const QString& playerName, QWidget *parent)
    : QWidget(parent), client(client), playerName(playerName) {

    setWindowTitle("Need For Speed 2D - Menu");
    resize(800, 600);

    // fondo de pantalla
    // QPixmap background(":/assets/need-for-speed/lobbyImg/wall3.jpg");
    QPixmap background("/home/alex/Documents/TP-Final-Taller_2C2025/v2.1/TP-Taller-G7/assets/need-for-speed/lobbyImg/wall3.jpg");

    if (!background.isNull()) {
        background = background.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPalette palette;
        palette.setBrush(QPalette::Window, QBrush(background));
        setAutoFillBackground(true);
        setPalette(palette);
    }

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignCenter);

    selectCarButton = new QPushButton("Select Car");
    createButton = new QPushButton("Create Game");
    joinButton   = new QPushButton("Join Game");

    for (auto btn : {selectCarButton, createButton, joinButton}) {
        btn->setFixedSize(200, 50);
        btn->setStyleSheet(
            "QPushButton {"
            " font-size: 18px;"
            " font-weight: bold;"
            " border-radius: 10px;"
            " background-color: rgba(255,0,0,0.8);"
            " color: white;"
            "}"
            "QPushButton:hover:enabled {"
            " background-color: rgba(255,60,60,1);"
            "}"
            "QPushButton:disabled {"
            " background-color: rgba(120,120,120,0.7);"
            " color: rgba(230,230,230,0.6);"
            " border: 1px solid rgba(80,80,80,0.9);"
            "}"
        );
        layout->addWidget(btn, 0, Qt::AlignCenter);
    }
}

QPushButton* LobbyMenuWindow::getCreateButton() { return createButton; }
QPushButton* LobbyMenuWindow::getJoinButton()   { return joinButton; }
QPushButton* LobbyMenuWindow::getSelectCarButton() { return selectCarButton; }
QString LobbyMenuWindow::getPlayerName() const { return playerName; }
