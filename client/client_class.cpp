#include "client_class.h"
#include <SDL.h>
#include <cstdio>

Client::Client(const char* host, const char* port) :
    protocol(host, port), snapshotQueue(100), eventQueue(10),
    receiver(protocol, snapshotQueue, eventQueue), playing(false) {}

void Client::run() {
    receiver.start();
    lobbyOptions();

    // Inicialización general (una sola vez)
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return;
    }

    const int W = 640, H = 480;
    SDL_Window* win = SDL_CreateWindow(
        "Cliente - Puntito",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 0);
    if (!win) {
        std::fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        std::fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return;
    }

    // Variables del juego
    int x = W / 2;
    int y = H / 2;

    bool running = true;

    Snapshot snapshot{};
    // Loop principal SDL
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_w: {
                        protocol.sendKey(SDLK_w);
                        break;
                    }
                    case SDLK_s: {
                        protocol.sendKey(SDLK_s);
                        break;
                    }
                    case SDLK_a: {
                        protocol.sendKey(SDLK_a);
                        break;
                    }
                    case SDLK_d: {
                        protocol.sendKey(SDLK_d);
                        break;
                    }
                }
            }
        }

        snapshotQueue.try_pop(snapshot);
        x = snapshot.posX;
        y = snapshot.posY;

        // Dibujar puntito
        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);

        SDL_Rect r{ x - 3, y - 3, 6, 6 };
        SDL_SetRenderDrawColor(ren, 240, 240, 240, 255);
        SDL_RenderFillRect(ren, &r);

        SDL_RenderPresent(ren);
    }

    // Limpiar al salir
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void Client::lobbyOptions() {
    std::string input;
    while (!playing) {
        std::getline(std::cin, input);
        playing = protocol.sendLobbyOption(input);

        if (playing) {
            Event event = eventQueue.pop();
            if (event.type == EventType::CREATE_JOIN_ACCEPTED) {
                playing = true;
            } else playing = false;
        }
    }
}
