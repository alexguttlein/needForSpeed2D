#include "client_class.h"
#include <SDL.h>
#include <cstdio>

Client::Client(const char* host, const char* port) :
    protocol(host, port) {
}

void Client::run() {
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

    // Comunicación inicial con el servidor
    uint8_t msg = protocol.recibir();
    if (msg == 0x00)
        std::cout << "recibido" << std::endl;

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
                        msg = 0x01;
                        protocol.send(msg);
                        uint8_t response = protocol.recibir();
                        if (response == 0x10) y -= 10;
                        break;
                    }
                    case SDLK_s: {
                        msg = 0x02;
                        protocol.send(msg);
                        uint8_t response = protocol.recibir();
                        if (response == 0x11) y += 10;
                        break;
                    }
                    case SDLK_a: {
                        msg = 0x03;
                        protocol.send(msg);
                        uint8_t response = protocol.recibir();
                        if (response == 0x12) x -= 10;
                        break;
                    }
                    case SDLK_d: {
                        msg = 0x04;
                        protocol.send(msg);
                        uint8_t response = protocol.recibir();
                        if (response == 0x13) x += 10;
                        break;
                    }
                }
            }
        }

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
