#include "client_class.h"
#include <SDL.h>
#include <cstdio>

Client::Client(const char* host, const char* port) :
    protocol(host, port), snapshotQueue(100),
    receiver(protocol, snapshotQueue) {}

void Client::run() {
    receiver.start();
    protocol.sendCreateGame(); //instruccion de prueba, esto va en el lobby

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

    // Loop principal SDL
    while (running) {
        Snapshot snapshot{};
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_w: {
                        protocol.send(SDLK_w);
                        break;
                    }
                    case SDLK_s: {
                        protocol.send(SDLK_s);
                        break;
                    }
                    case SDLK_a: {
                        protocol.send(SDLK_a);
                        break;
                    }
                    case SDLK_d: {
                        protocol.send(SDLK_d);
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
