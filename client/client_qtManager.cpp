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
    window->setWindowTitle("Need For Speed 2D");
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

    // Input de nombre
    QLineEdit* nameInput = new QLineEdit(center);
    nameInput->setPlaceholderText("Enter your driver name");
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
    QPushButton* startButton = new QPushButton("START ENGINE", center);
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
        window->close();
        showLobbyWindow(playerName);
    });

    centerLayout->addWidget(nameInput);
    centerLayout->addSpacing(20);
    centerLayout->addWidget(startButton);
    center->setLayout(centerLayout);

    layout->addStretch();
    layout->addWidget(center, 0, Qt::AlignCenter);
    layout->addStretch();

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

        if (!client->getProtocol().sendLobbyOption("crear")) {
            QMessageBox::warning(lobby, "Error", "No se pudo enviar la solicitud al servidor.");
            return;
        }

        auto* waiting = new WaitingWindow(client->getEventQueue());
        waiting->show();
        lobby->hide();

        (void)QtConcurrent::run([this, lobby, waiting]() {
            Event event = client->getEventQueue().pop();

            QMetaObject::invokeMethod(waiting, [lobby, waiting, event, this]() {
                waiting->close();
                if (event.type == EventType::CREATE_JOIN_ACCEPTED) {
                    if (!event.message.empty()) {
                        client->setSelfId(std::stoi(event.message));
                        qDebug() << "Client selfId set to:" << std::stoi(event.message);
                    }
                    lobby->close();
                } else {
                    QMessageBox::information(lobby, "Error", "No se pudo crear la partida.");
                    lobby->show();
                }
            }, Qt::QueuedConnection);
        });
    });
}

void ClientQtManager::setupJoinButton(LobbyMenuWindow* lobby) {
    QObject::connect(lobby->getJoinButton(), &QPushButton::clicked, [this, lobby]() {
        // deshabilitar UI mientras pedimos la lista
        lobby->getCreateButton()->setEnabled(false);
        lobby->getJoinButton()->setEnabled(false);
        lobby->getSelectCarButton()->setEnabled(false);

        client->getProtocol().sendLobbyOption("listar");

        QTimer::singleShot(500, [this, lobby]() {
            Snapshot snapshot{};
            bool received = client->getSnapshotQueue().try_pop(snapshot);

            // reactivar botones
            lobby->getCreateButton()->setEnabled(true);
            lobby->getJoinButton()->setEnabled(true);
            lobby->getSelectCarButton()->setEnabled(true);

            if (!received || snapshot.gameList.empty()) {
                QMessageBox::information(lobby, "Info", "No hay partidas activas.");
                return;
            }

            auto* listWindow = new GameListWindow(client);
            listWindow->loadGameList(snapshot.gameList);
            listWindow->show();

            QObject::connect(listWindow, &GameListWindow::gameSelected, [this, lobby, listWindow](uint32_t gameId) {
                if (!client->getProtocol().sendLobbyOption("unirse " + std::to_string(gameId))) {
                    QMessageBox::warning(lobby, "Error", "No se pudo enviar la solicitud de unirse.");
                    return;
                }

                auto* waiting = new WaitingWindow(client->getEventQueue());
                waiting->show();
                listWindow->close();
                lobby->hide();

                (void)QtConcurrent::run([this, lobby, waiting]() {
                    Event event = client->getEventQueue().pop();
                    QMetaObject::invokeMethod(waiting, [lobby, waiting, event, this]() {
                        waiting->close();
                        if (event.type == EventType::CREATE_JOIN_ACCEPTED) {
                            if (!event.message.empty()) {
                                client->setSelfId(std::stoi(event.message));
                                qDebug() << "Client selfId set to:" << std::stoi(event.message);
                            }
                            lobby->close();
                        } else {
                            QMessageBox::warning(lobby, "Error", "No se pudo unirse a la partida.");
                            lobby->show();
                        }
                    }, Qt::QueuedConnection);
                });
            });
        });
    });
}

void ClientQtManager::setupSelectCarButton(LobbyMenuWindow* lobby) {
    QObject::connect(lobby->getSelectCarButton(), &QPushButton::clicked, [this, lobby]() {
        // Abrir ventana de selección de auto
        auto* carWindow = new CarSelectionWindow(lobby);
        QObject::connect(carWindow, &CarSelectionWindow::carChosen, [lobby](int carId) {
            lobby->carChosen = true; // actualizar flag
            lobby->getCreateButton()->setEnabled(true);
            lobby->getJoinButton()->setEnabled(true);
            std::cout << "el id elegido es " << carId << std::endl;
        });
        carWindow->show();
    });

    // Inicialmente deshabilitar botones de juego
    lobby->getCreateButton()->setEnabled(false);
    lobby->getJoinButton()->setEnabled(false);
}
