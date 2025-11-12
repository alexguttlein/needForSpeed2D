#include "lobbyMenuWindow.h"
#include "client_class.h"
#include "common/eventType.h"
#include "waitingWindow.h"

LobbyMenuWindow::LobbyMenuWindow(Client* client, const QString& playerName, QWidget *parent)
    : QWidget(parent), client(client), playerName(playerName) {

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
            "QPushButton:hover {"
            " background-color: rgba(255,60,60,0.9);"
            "}"
        );
        layout->addWidget(btn, 0, Qt::AlignCenter);
    }
    // conectar botones
    connect(createButton, &QPushButton::clicked, this, &LobbyMenuWindow::onCreateGameClicked);
    connect(joinButton, &QPushButton::clicked, this, &LobbyMenuWindow::onJoinGameClicked);
}

QPushButton* LobbyMenuWindow::getCreateButton() { return createButton; }
QPushButton* LobbyMenuWindow::getJoinButton()   { return joinButton; }
QPushButton* LobbyMenuWindow::getSelectCarButton() { return selectCarButton; }
QString LobbyMenuWindow::getPlayerName() const { return playerName; }

void LobbyMenuWindow::onCreateGameClicked() {
    // Deshabilitamos los botones mientras esperamos
    createButton->setEnabled(false);
    joinButton->setEnabled(false);
    selectCarButton->setEnabled(false);

    // Mostrar ventana temporal "Waiting for server response..."
    auto waitingWindow = new WaitingWindow(playerName);
    waitingWindow->show();
    this->hide();

    // Esperar en otro hilo a que el servidor responda
    (void)QtConcurrent::run([this, waitingWindow]() {
        // se bloquea hasta tener respuesta del server
        Event event = client->getEventQueue().pop();

        QMetaObject::invokeMethod(waitingWindow, [this, waitingWindow, event]() {
            if (event.type == EventType::CREATE_JOIN_ACCEPTED) {
                // Crear partida aceptada
                waitingWindow->close();
                this->close();
            } else {
                // No se pudo crear la partida
                waitingWindow->close();
                this->show();
                QMessageBox::information(this, "Error", "No se pudo crear la partida.");
                createButton->setEnabled(true);
                joinButton->setEnabled(true);
                selectCarButton->setEnabled(true);
            }
        }, Qt::QueuedConnection);
    });
}

void LobbyMenuWindow::onJoinGameClicked() {
    // Deshabilitar botones mientras cargamos
    createButton->setEnabled(false);
    joinButton->setEnabled(false);
    selectCarButton->setEnabled(false);

    // Pedir la lista de juegos al servidor
    client->getProtocol().sendLobbyOption("listar");

    // Recibir lista de partidas activas
    auto snapshotOpt = client->getSnapshotQueue().pop();
    if (!snapshotOpt.gameList.data()) {
        QMessageBox::information(this, "Info", "No hay partidas activas.");
        createButton->setEnabled(true);
        joinButton->setEnabled(true);
        selectCarButton->setEnabled(true);
        return;
    }

    // Abrir ventana con lista de partidas
    auto gameListWindow = new GameListWindow(client);
    gameListWindow->loadGameList(snapshotOpt.gameList);

    connect(gameListWindow, &GameListWindow::gameSelected, [this](uint32_t gameId) {
        // Enviar al servidor la orden de unirse a la partida
        bool sent = client->getProtocol().sendLobbyOption("unirse " + std::to_string(gameId));
        if (!sent) {
            QMessageBox::warning(this, "Error", "No se pudo enviar la solicitud de unirse.");
            return;
        }

        // Esperar la respuesta del server en otro hilo
        (void)QtConcurrent::run([this]() {
            Event event = client->getEventQueue().pop();
            QMetaObject::invokeMethod(this, [this, event]() {
                if (event.type == EventType::CREATE_JOIN_ACCEPTED) {
                    // partida aceptada, cerramos el lobby
                    this->close();
                } else {
                    QMessageBox::warning(this, "Error", "No se pudo unirse a la partida.");
                    createButton->setEnabled(true);
                    joinButton->setEnabled(true);
                    selectCarButton->setEnabled(true);
                }
            }, Qt::QueuedConnection);
        });
    });
    gameListWindow->show();

    // se vuelven a activar los botones en caso de que el user haya cancelado el join
    createButton->setEnabled(true);
    joinButton->setEnabled(true);
    selectCarButton->setEnabled(true);
}