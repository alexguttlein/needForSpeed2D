#include "lobbyMenuWindow.h"

LobbyMenuWindow::LobbyMenuWindow(const QString& playerName, QWidget *parent)
    : QWidget(parent), playerName(playerName) {

    setWindowTitle("Need For Speed 2D - Menu");
    resize(800, 600);

    // fondo de pantalla
    QPixmap background(":/assets/need-for-speed/lobbyImg/wall3.jpg");

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

    createButton = new QPushButton("Crear Partida");
    joinButton   = new QPushButton("Unirse a Partida");
    selectCarButton = new QPushButton("Elegir Auto");

    for (auto btn : {createButton, joinButton, selectCarButton}) {
        btn->setFixedSize(200, 50);
        btn->setStyleSheet(
            "QPushButton {"
            " font-size: 18px;"
            " font-weight: bold;"
            " border-radius: 10px;"
            " background-color: rgba(255,0,0,0.8);"
            " color: white;"
            "}"
            "QPushButton:hover {"
            " background-color: rgba(255,60,60,0.9);"
            "}"
        );
        layout->addWidget(btn, 0, Qt::AlignCenter);
    }
}

QPushButton* LobbyMenuWindow::getCreateButton() { return createButton; }
QPushButton* LobbyMenuWindow::getJoinButton()   { return joinButton; }
QPushButton* LobbyMenuWindow::getSelectCarButton() { return selectCarButton; }
QString LobbyMenuWindow::getPlayerName() const { return playerName; }
