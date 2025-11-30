#include "client_qtManager.h"
#include "client_class.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QPalette>
#include <QPixmap>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>

ClientQtManager::ClientQtManager(Client* client)
    : client(client), app(nullptr) {}

ClientQtManager::~ClientQtManager() = default;

void ClientQtManager::start() {
    int argc = 0;
    char** argv = nullptr;
    app = std::make_unique<QApplication>(argc, argv);
    showLoginWindow();
    app->exec();
}

void ClientQtManager::showLoginWindow() {
    QWidget* window = new QWidget();
    window->setWindowTitle(Constants::NFS_TITLE.data());
    window->resize(800, 600);

    // Fondo de pantalla
    QPixmap background("assets/need-for-speed/lobbyImg/wall3.jpg");
    if (!background.isNull()) {
        background = background.scaled(window->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPalette palette;
        palette.setBrush(QPalette::Window, background);
        window->setAutoFillBackground(true);
        window->setPalette(palette);
    }

    // Layout central
    QVBoxLayout* layout = new QVBoxLayout(window);
    QWidget* center = new QWidget(window);
    QVBoxLayout* centerLayout = new QVBoxLayout(center);

    // título
    QLabel* title = new QLabel(Constants::NFS_TITLE.data(), window);
    title->setAlignment(Qt::AlignCenter);
    title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    title->setStyleSheet(
        "QLabel {"
        " color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 red, stop:1 white);"
        " font-size: 65px;"
        " font-weight: bold;"
        " font-family: 'Verdana';"
        "}"
    );

    auto* glow = new QGraphicsDropShadowEffect(title);
    glow->setBlurRadius(80);
    glow->setColor(QColor(255, 0, 0));
    glow->setOffset(0);
    title->setGraphicsEffect(glow);

    // Input de nombre
    QLineEdit* nameInput = new QLineEdit(center);
    nameInput->setPlaceholderText(Constants::ENTER_DRIVER_NAME.data());
    nameInput->setMinimumWidth(400);
    nameInput->setStyleSheet(
        "QLineEdit {"
        " color: white;"
        " font-size: 18px;"
        " font-weight: bold;"
        " padding: 10px 15px;"
        " border: 2px solid rgba(255, 0, 0, 0.6);"
        " border-radius: 10px;"
        " background-color: rgba(0, 0, 0, 0.9);"
        " selection-background-color: rgba(255, 0, 0, 0.6);"
        "}"
        "QLineEdit:focus {"
        " border: 2px solid rgba(255, 60, 60, 0.9);"
        " background-color: rgba(0, 0, 0, 0.95);"
        "}"
    );

    // Botón start
    QPushButton* startButton = new QPushButton(Constants::START_BUTTON.data(), center);
    startButton->setEnabled(false);
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setStyleSheet(
        "QPushButton {"
        " color: white;"
        " font-size: 18px;"
        " font-weight: bold;"
        " padding: 10px;"
        " border-radius: 10px;"
        " background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        " stop:0 rgba(255, 0, 0, 0.9), stop:1 rgba(120, 0, 0, 0.9));"
        "}"
        "QPushButton:hover:!disabled {"
        " background-color: rgba(255, 0, 0, 1);"
        " border: 2px solid white;"
        "}"
        "QPushButton:disabled {"
        " background-color: rgba(60, 60, 60, 0.8);"
        " color: rgba(180, 180, 180, 0.6);"
        "}"
    );

    // Efecto neón rojo
    auto* neonEffect = new QGraphicsDropShadowEffect(window);
    neonEffect->setBlurRadius(40);
    neonEffect->setColor(QColor(255, 0, 0));
    neonEffect->setOffset(0, 0);
    startButton->setGraphicsEffect(neonEffect);

    auto* neonAnimation = new QPropertyAnimation(neonEffect, "blurRadius");
    neonAnimation->setDuration(1500);
    neonAnimation->setStartValue(20);
    neonAnimation->setEndValue(60);
    neonAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    neonAnimation->setLoopCount(-1);

    QObject::connect(nameInput, &QLineEdit::textChanged, [nameInput, startButton, neonAnimation]() {
        bool hasName = !nameInput->text().isEmpty();
        startButton->setEnabled(hasName);
        hasName ? neonAnimation->start() : neonAnimation->stop();
    });

    // Cuando se presiona START ENGINE
    QObject::connect(startButton, &QPushButton::clicked, [this, window, nameInput]() {
        QString playerName = nameInput->text();
        client->setPlayerName(playerName.toStdString()); //paso el nombre al client
        window->close();
        showLobbyWindow(playerName);
    });

    layout->setContentsMargins(0, 20, 0, 50); // margen general
    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(100);
    layout->addWidget(center, 0, Qt::AlignHCenter);
    layout->addStretch();

    centerLayout->addWidget(nameInput);
    centerLayout->addSpacing(20);
    centerLayout->addWidget(startButton);
    center->setLayout(centerLayout);

    window->setLayout(layout);
    window->show();
}

void ClientQtManager::showLobbyWindow(const QString& playerName) {
    auto* lobby = new LobbyMenuWindow(client, playerName);
    lobby->show();

    setupCreateButton(lobby);
    setupJoinButton(lobby);
    setupSelectCarButton(lobby);
}

void ClientQtManager::setupCreateButton(LobbyMenuWindow* lobby) {
    QObject::connect(lobby->getCreateButton(), &QPushButton::clicked, [this, lobby]() {

        if (!client->sendLobbyOption(Constants::INPUT_CREAR,
            client->getPlayerName(), client->getSelectedCar())) {
            QMessageBox::warning(lobby, Constants::ERROR_TXT.data(),
                Constants::NO_ENVIO_SOLICITUD.data());
            return;
        }

        auto* waiting = new WaitingWindow(true); //el flag indica que es el creador
        waiting->show();
        lobby->hide();

        waitForGameEvents(waiting, lobby);
    });
}

void ClientQtManager::setupJoinButton(LobbyMenuWindow* lobby) {
    QObject::connect(lobby->getJoinButton(), &QPushButton::clicked, [this, lobby]() {
        // deshabilitar UI mientras pedimos la lista
        lobby->getCreateButton()->setEnabled(false);
        lobby->getJoinButton()->setEnabled(false);
        lobby->getSelectCarButton()->setEnabled(false);

        client->sendLobbyOption(Constants::INPUT_LISTAR,"",0);

        QTimer::singleShot(500, [this, lobby]() {
            Snapshot snapshot{};
            bool received = client->getSnapshotQueue().try_pop(snapshot);

            // reactivar botones
            lobby->getCreateButton()->setEnabled(true);
            lobby->getJoinButton()->setEnabled(true);
            lobby->getSelectCarButton()->setEnabled(true);

            if (!received || snapshot.gameList.empty()) {
                QMessageBox::information(lobby, Constants::INFO_TXT.data(),
                    Constants::NO_HAY_PARTIDAS.data());
                return;
            }

            auto* listWindow = new GameListWindow(client);
            listWindow->loadGameList(snapshot.gameList);
            listWindow->show();

            QObject::connect(listWindow, &GameListWindow::gameSelected, [this, lobby, listWindow](uint32_t gameId) {
                if (!client->sendLobbyOption(Constants::INPUT_UNIRSE + " " + std::to_string(gameId),
                        client->getPlayerName(), client->getSelectedCar())) {
                    QMessageBox::warning(lobby, Constants::ERROR_TXT.data(),
                        Constants::NO_ENVIO_SOLICITUD_UNIRSE.data());
                    return;
                }

                auto* waiting = new WaitingWindow(false); //el flag indica que no es el creador
                waiting->show();
                listWindow->close();
                lobby->hide();

                waitForGameEvents(waiting, lobby);
            });
        });
    });
}

void ClientQtManager::setupSelectCarButton(LobbyMenuWindow* lobby) {
    QObject::connect(lobby->getSelectCarButton(), &QPushButton::clicked, [this, lobby]() {
        // Abrir ventana de selección de auto
        auto* carWindow = new CarSelectionWindow(lobby);
        QObject::connect(carWindow, &CarSelectionWindow::carChosen, [this, lobby](int carId) {
            client->setSelectedCar(carId); //se informa al cliente el auto elegido

            lobby->carChosen = true; // actualizar flag
            lobby->getCreateButton()->setEnabled(true);
            lobby->getJoinButton()->setEnabled(true);
        });
        carWindow->show();
    });

    // Inicialmente deshabilitar botones de juego
    lobby->getCreateButton()->setEnabled(false);
    lobby->getJoinButton()->setEnabled(false);
}

void ClientQtManager::waitForGameEvents(WaitingWindow* waiting, LobbyMenuWindow* lobby) {

    //solo se conectan los botones una vez
    static bool buttonsConnected = false;
    if (!buttonsConnected) {
        buttonsConnected = true;

        // Cancel button
        QObject::connect(waiting, &WaitingWindow::cancelled, [lobby, waiting]() {
            lobby->show();
            waiting->close();
        });

        // start game button
        QObject::connect(waiting, &WaitingWindow::startGamePressed, [this, lobby, waiting]() {

            bool ok = client->sendLobbyOption(Constants::INPUT_START_GAME,
                                              client->getPlayerName(),
                                              client->getGameId());

            if (!ok) {
                QMessageBox::warning(waiting, Constants::ERROR_TXT.data(),
                                     Constants::START_GAME_ERROR.data());
                return;
            }

            waiting->close();
            lobby->close();
        });
    }

    // Hilo que bloquea en pop()
    (void)QtConcurrent::run([this, waiting, lobby]() {

        Event event = client->getEventQueue().pop();

        QMetaObject::invokeMethod(waiting, [this, event, waiting, lobby]() {
            // evento de jugador aceptado
            if (event.type == EventType::CREATE_JOIN_ACCEPTED) {

                if (!event.message.empty()) {
                    client->setSelfId(std::stoi(event.message));
                }

                if (!event.auxMessage.empty()) {
                    client->setGameId(std::stoi(event.auxMessage));
                    waiting->setGameInfo(client->getGameId());
                }

                // Esperar el segundo evento, SIN cerrar waiting
                waitForGameEvents(waiting, lobby);
                return;
            }

            // evento para comenzar partida
            if (event.type == EventType::GAME_START) {
                waiting->close();
                lobby->close();
                client->changePlayingStatus();
                emit waiting->gameShouldStart();
                return;
            }

            // ERROR
            QMessageBox::warning(lobby, Constants::ERROR_TXT.data(),
                                 Constants::SERVER_ERROR.data());
            lobby->close();
            waiting->close();
            app->quit();
        }, Qt::QueuedConnection);
    });
}
