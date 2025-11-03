#include "client_dibujador.h"
#include <SDL_image.h>
#include <cmath>
#include <algorithm>

ClientDibujador::ClientDibujador(SDL_Renderer* r, int w, int h)
    : ren(r), winW(w), winH(h) {}

ClientDibujador::~ClientDibujador() {
    if (carTex) SDL_DestroyTexture(carTex);
    if (mapTex) SDL_DestroyTexture(mapTex);
}

SDL_Texture* ClientDibujador::loadTexture_(const std::string& path) {
    SDL_Texture* tex = IMG_LoadTexture(ren, path.c_str());
    if (!tex) {
        SDL_Log("IMG_LoadTexture('%s'): %s", path.c_str(), IMG_GetError());
        return nullptr;
    }
    return tex;
}

bool ClientDibujador::loadMap(const std::string& pathPng) {
    mapTex = loadTexture_(pathPng);
    if (!mapTex) return false;
    SDL_QueryTexture(mapTex, nullptr, nullptr, &mapW, &mapH);
    return true;
}

bool ClientDibujador::loadCarAtlas(const std::string& pathPng, int cols, int rows,
                             float angle0Deg, bool cw) {
    carTex = loadTexture_(pathPng);
    if (!carTex) return false;
    atlasCols = cols; atlasRows = rows;
    angle0 = angle0Deg; clockwise = cw;

    int w, h;
    SDL_QueryTexture(carTex, nullptr, nullptr, &w, &h);
    cellW = w / atlasCols;
    cellH = h / atlasRows;
    return true;
}

static inline float norm360(float a) {
    while (a < 0)   a += 360.f;
    while (a >= 360.f) a -= 360.f;
    return a;
}

int ClientDibujador::frameForAngle_(float angleDeg) const {
    float step = 360.0f / float(atlasCols * atlasRows);
    float rel = norm360(angleDeg - angle0);
    int idx = int(std::floor((rel + step/2.f) / step)) % (atlasCols * atlasRows);
    if (!clockwise) {
        if (idx == 0) return 0;
        idx = (atlasCols*atlasRows) - idx;
        idx %= (atlasCols*atlasRows);
    }
    return idx;
}

void ClientDibujador::updateCamera_(int px, int py) {
    camX = px - winW / 2;
    camY = py - winH / 2;
    if (camX < 0) camX = 0;
    if (camY < 0) camY = 0;
    if (mapTex) {
        camX = std::min(camX, std::max(0, mapW - winW));
        camY = std::min(camY, std::max(0, mapH - winH));
    }
}

void ClientDibujador::renderFrame(int playerX, int playerY) {
    if (lastX >= 0 && lastY >= 0) {
        int dx = playerX - lastX;
        int dy = playerY - lastY;
        const float eps = 0.5f;
        if (std::abs(dx) > eps || std::abs(dy) > eps) {
            facingDeg = std::atan2(float(dy), float(dx)) * 180.0f / float(M_PI);
        }
    }
    lastX = playerX; lastY = playerY;
    updateCamera_(playerX, playerY);

    SDL_SetRenderDrawColor(ren, 20,20,20,255);
    SDL_RenderClear(ren);

    if (mapTex) {
        SDL_Rect src{ camX, camY, winW, winH };
        SDL_Rect dst{ 0, 0, winW, winH };
        SDL_RenderCopy(ren, mapTex, &src, &dst);
    }

    if (carTex && cellW > 0 && cellH > 0) {
        int frame = frameForAngle_(facingDeg);
        int col = frame % atlasCols;
        int row = frame / atlasCols;
        SDL_Rect src{ col * cellW, row * cellH, cellW, cellH };
        int sx = playerX - camX - cellW/2;
        int sy = playerY - camY - cellH/2;
        SDL_Rect dst{ sx, sy, cellW, cellH };
        SDL_RenderCopy(ren, carTex, &src, &dst);
    }

    SDL_RenderPresent(ren);
}
