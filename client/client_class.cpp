#include "client_class.h"
#include "client_dibujador.h"

#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>
#include <chrono>

using ms = std::chrono::milliseconds;
constexpr int FPS = 30;
const ms FRAME_MS {1000 / FPS };

Client::Client(const char* host, const char* port) :
    protocol(host, port), snapshotQueue(100), commandQueue(100), eventQueue(10),
    receiver(protocol, snapshotQueue, eventQueue), sender(protocol, commandQueue), playing(false) {}

void Client::run() {
    receiver.start();
    sender.start();
    goToLobby();
    // lobbyOptions();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return;
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
        std::fprintf(stderr, "IMG_Init PNG error: %s\n", IMG_GetError());
        SDL_Quit(); return;
    }

    const int W = 640, H = 480;
    SDL_Window* win = SDL_CreateWindow(
        "Cliente - Mapa + Auto",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 0);
    if (!win) {
        std::fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        std::fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    ClientDibujador dib(ren, W, H);

    if (!dib.loadMap("/home/alex/Documents/TP-Final-Taller_2C2025/v2.1/TP-Taller-G7/assets/need-for-speed/cities/Liberty.png")) {
        std::fprintf(stderr, "No pude cargar assets/maps/iberty.png\n");
    }

    if (!dib.loadCarAtlas("/home/alex/Documents/TP-Final-Taller_2C2025/v2.1/TP-Taller-G7/assets/need-for-speed/cars/auto-1.png", 8, 2, 0.0f, true)) {
        std::fprintf(stderr, "No pude cargar atlas del auto\n");
    }

    dib.setFacingDeg(0.0f);
    bool running = true;
    //CAMBIAR
    // int x = 90, y = 90 ;
    bool havePos = false;

    // bool haveSnapshot = false;
    Snapshot snapshot;
    int selfId = -1;

    while (running) {
        auto start = std::chrono::steady_clock::now();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: running = false; break;
                    case SDLK_w: commandQueue.push(SDLK_w); break;
                    case SDLK_s: commandQueue.push(SDLK_s); break;
                    case SDLK_a: commandQueue.push(SDLK_a); break;
                    case SDLK_d: commandQueue.push(SDLK_d); break;
                }
            }
        }

        Snapshot snapTmp;
        if (snapshotQueue.try_pop(snapTmp)) {
            snapshot = std::move(snapTmp);
            havePos = true;
            if (selfId == -1 && snapshot.playerId) selfId = snapshot.playerId; // solo la primera vez
        }

        if (havePos) {
            dib.renderAll(snapshot.players, selfId);
        }

        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<ms>(end - start);
        if (elapsed < FRAME_MS) {
            std::this_thread::sleep_for(FRAME_MS - elapsed);
        }
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
}

Queue<Event>& Client::getEventQueue() {
    return eventQueue;
}

ClientProtocol& Client::getProtocol() {
    return protocol;
}

Queue<Snapshot> & Client::getSnapshotQueue() {
    return snapshotQueue;
}

void Client::goToLobby() {

    int argc = 0;
    char **argv = nullptr;

    QApplication app(argc, argv);

    // se crea ventana de qt
    QWidget window;
    window.setWindowTitle("Need For Speed 2D");
    window.resize(800, 600);

    // fondo de pantalla
   QPixmap background(":/assets/need-for-speed/lobbyImg/wall3.jpg");

    if (!background.isNull()) {
        background = background.scaled(window.size(), Qt::KeepAspectRatioByExpanding);
        QPalette palette;
        palette.setBrush(QPalette::Window, background);
        window.setAutoFillBackground(true);
        window.setPalette(palette);
    }

    auto *mainLayout = new QVBoxLayout(&window);
    //se agregan espaciadores para centrar vertical
    mainLayout->addSpacerItem(new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // se agrega contenedor central para limitar ancho
    QWidget *centerWidget = new QWidget(&window);
    auto *centerLayout = new QVBoxLayout(centerWidget);
    centerWidget->setFixedWidth(350);  // ancho limitado

    auto *nameInput = new QLineEdit(centerWidget);
    nameInput->setPlaceholderText("Enter your driver name");
    nameInput->setStyleSheet(
        "QLineEdit {"
        " color: white;"   //texto ingresado
        " font-size: 18px;"
        " font-weight: bold;"
        " padding: 10px 15px;"
        " border: 2px solid rgba(255, 0, 0, 0.6);"
        " border-radius: 10px;"
        " background-color: rgba(0, 0, 0, 0.9);"  //fondo oscuro
        " selection-background-color: rgba(255, 0, 0, 0.6);"
        "}"
        "QLineEdit:focus {"
        " border: 2px solid rgba(255, 60, 60, 0.9);"
        " background-color: rgba(0, 0, 0, 0.95);"
        "}"
        "QLineEdit::placeholderText {"
        " color: rgba(200, 200, 200, 0.5);"
        "}"
    );

    auto *continueButton = new QPushButton("START ENGINE", centerWidget);
    continueButton->setEnabled(false); //deshabilitado al iniciar
    continueButton->setCursor(Qt::PointingHandCursor);
    continueButton->setStyleSheet(
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

    //efecto neón
    auto *neonEffect = new QGraphicsDropShadowEffect(&window);
    neonEffect->setBlurRadius(40);
    neonEffect->setColor(QColor(255, 0, 0));
    neonEffect->setOffset(0, 0);
    continueButton->setGraphicsEffect(neonEffect);

    auto *neonAnimation = new QPropertyAnimation(neonEffect, "blurRadius");
    neonAnimation->setDuration(1500);
    neonAnimation->setStartValue(20);
    neonAnimation->setEndValue(60);
    neonAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    neonAnimation->setLoopCount(-1);

    // iniciar animación solo cuando esté habilitado
    QObject::connect(nameInput, &QLineEdit::textChanged, [&] {
        bool hasName = !nameInput->text().isEmpty();
        continueButton->setEnabled(hasName);
        if (hasName)
            neonAnimation->start();
        else
            neonAnimation->stop();
    });

    centerLayout->addWidget(nameInput);
    centerLayout->addSpacing(20);
    centerLayout->addWidget(continueButton);
    centerWidget->setLayout(centerLayout);

    //se agrega al layout principal centrado
    mainLayout->addWidget(centerWidget, 0, Qt::AlignHCenter);
    mainLayout->addSpacerItem(new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding));

    window.setLayout(mainLayout);
    window.show();

    // habilitar botón solo si hay texto
    QObject::connect(nameInput, &QLineEdit::textChanged, [&] {
        continueButton->setEnabled(!nameInput->text().isEmpty());
    });

    // cuando se presiona "START ENGINE" se va a otra pantalla
    QObject::connect(continueButton, &QPushButton::clicked, [&] {
        playerName = nameInput->text().toStdString();
        QString name = nameInput->text();

        LobbyMenuWindow *lobbyMenu = new LobbyMenuWindow(this, name);
        lobbyMenu->show();

    // conectar la señal del botón "Crear partida"
    QObject::connect(lobbyMenu->getCreateButton(), &QPushButton::clicked, [this, lobbyMenu]() {
        // Se envía la orden "crear" al servidor
        bool sent = this->protocol.sendLobbyOption("crear");

        if (sent)
            lobbyMenu->close();
    });
        window.close(); //cierra la ventana
    });

    // se inicia qt
    app.exec();
}
