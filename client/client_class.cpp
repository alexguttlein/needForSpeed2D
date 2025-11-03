#include "client_class.h"
#include "client_dibujador.h"

#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>

Client::Client(const char* host, const char* port) :
    protocol(host, port), snapshotQueue(100), eventQueue(10),
    receiver(protocol, snapshotQueue, eventQueue), playing(false) {}

void Client::run() {
    receiver.start();
    lobbyOptions();

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

    if (!dib.loadMap("assets/need-for-speed/cities/Liberty.png")) {
        std::fprintf(stderr, "No pude cargar assets/maps/iberty.png\n");
    }

    if (!dib.loadCarAtlas("assets/need-for-speed/cars/auto-1.png", 8, 2, -90.0f, true)) {
        std::fprintf(stderr, "No pude cargar atlas del auto\n");
    }

    bool running = true;
    int x = W/2, y = H/2;
    int lastX = x, lastY = y;

    while (running) {
        Snapshot snapshot{};
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: running = false; break;
                    case SDLK_w: protocol.sendKey(SDLK_w); break;
                    case SDLK_s: protocol.sendKey(SDLK_s); break;
                    case SDLK_a: protocol.sendKey(SDLK_a); break;
                    case SDLK_d: protocol.sendKey(SDLK_d); break;
                }
            }
        }

        if (snapshotQueue.try_pop(snapshot)) {
            x = snapshot.posX;
            y = snapshot.posY;
        } else {
            x = lastX; y = lastY;
        }
        lastX = x; lastY = y;
        dib.renderFrame(x, y);
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
}

void Client::lobbyOptions() {
    std::string input;
    while (!playing) {
        std::getline(std::cin, input);
        playing = protocol.sendLobbyOption(input);

        if (playing) {
            Event event = eventQueue.pop();
            playing = (event.type == EventType::CREATE_JOIN_ACCEPTED);
        }
    }
}
