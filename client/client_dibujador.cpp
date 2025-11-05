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
    while (a < 0.0f)   a += 360.f;
    while (a >= 360.f) a -= 360.f;
    return a;
}

static inline float shortestsDelta(float fromDeg, float toDeg) {
    float d = norm360(toDeg - fromDeg);
    if (d > 180.0f) d -= 360.0f;
    return d;
}

static inline float approachAngle(float curDeg, float targetDeg, float maxStepDeg) {
    float d = shortestsDelta(curDeg, targetDeg);
    if (std::fabs(d) <= maxStepDeg) return norm360(targetDeg);
    return norm360(curDeg + (d > 0 ? maxStepDeg : -maxStepDeg));
}

int ClientDibujador::frameForAngle_(float angleDeg) const {
    const int total = atlasCols * atlasRows;
    const float step = 360.0f / static_cast<float>(total);
    float rel = clockwise ? (angleDeg - angle0) : (angle0 - angleDeg);
    rel = norm360(rel);
    const int idx = static_cast<int>(std::floor((rel + step * 0.5f) / step)) % (total);
    return idx;
}



void ClientDibujador::updateCamera_(int px, int py) {
    // Tamaño de la banda muerta (25% de la ventana)
    const int marginX = winW / 4;
    const int marginY = winH / 4;

    // Rectángulo visible actual
    int left   = camX + marginX;
    int right  = camX + winW - marginX;
    int top    = camY + marginY;
    int bottom = camY + winH - marginY;

    // Mover cam lo mínimo necesario para mantener al jugador dentro de la banda
    if (px < left)   camX -= (left - px);
    if (px > right)  camX += (px - right);
    if (py < top)    camY -= (top - py);
    if (py > bottom) camY += (py - bottom);

    // Limitar a los bordes del mapa
    if (mapW > 0 && mapH > 0) {
        camX = std::clamp(camX, 0, std::max(0, mapW - winW));
        camY = std::clamp(camY, 0, std::max(0, mapH - winH));
    } else {
        camX = std::max(0, camX);
        camY = std::max(0, camY);
    }
}

void ClientDibujador::renderFrame(int playerX, int playerY) {
    // Usamos mundo puro: sin START_OFFSET ni TILE hacks
    int px = playerX;
    int py = playerY;

    // Ángulo de frente según el delta real
    static float target = 0.0f;
    if (lastX >= 0 && lastY >= 0) {
        int dx = px - lastX, dy = py - lastY;
        if (dx || dy) {
            float hd = std::atan2(float(dy), float(dx)) * 180.0f / float(M_PI);
            if (hd < 0.f) hd += 360.f;
            target = hd;
        }
    }
    lastX = px; lastY = py;
    const int totalDirs = atlasCols * atlasRows;
    const float stepDEg = 360.0f / float(totalDirs);
    facingDeg = approachAngle(facingDeg, target, stepDEg);

    updateCamera_(px, py);

    SDL_SetRenderDrawColor(ren, 20,20,20,255);
    SDL_RenderClear(ren);

    if (mapTex) {
        SDL_Rect src{ camX, camY, winW, winH };
        SDL_Rect dst{ 0, 0, winW, winH };
        SDL_RenderCopy(ren, mapTex, &src, &dst);
    }

    if (carTex && cellW > 0 && cellH > 0) {
        int frame = frameForAngle_(facingDeg);
        int col = frame % atlasCols, row = frame / atlasCols;
        SDL_Rect s{ col * cellW, row * cellH, cellW, cellH };
        SDL_Rect d{
            px - camX - cellW/2,
            py - camY - cellH/2,
            cellW, cellH
        };
        SDL_RenderCopy(ren, carTex, &s, &d);
    }

    SDL_RenderPresent(ren);
}

void ClientDibujador::renderAll(const std::vector<Player>& players, int selfId) {
    SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
    SDL_RenderClear(ren);

    if (mapTex) {
        SDL_Rect src{ camX, camY, winW, winH };
        SDL_Rect dst{ 0, 0, winW, winH };
        SDL_RenderCopy(ren, mapTex, &src, &dst);
    }

    for (const auto& p : players) {
        int px = p.posX;
        int py = p.posY;

        // actualizar cámara centrada en tu propio jugador
        if (p.playerId == selfId) {
            updateCamera_(px, py);
        }

        if (carTex && cellW > 0 && cellH > 0) {
            int frame = frameForAngle_(facingDeg);
            int col = frame % atlasCols, row = frame / atlasCols;
            SDL_Rect s{ col * cellW, row * cellH, cellW, cellH };
            SDL_Rect d{
                px - camX - cellW / 2,
                py - camY - cellH / 2,
                cellW, cellH
            };

            SDL_RenderCopy(ren, carTex, &s, &d);
        }
    }

    SDL_RenderPresent(ren);
}
