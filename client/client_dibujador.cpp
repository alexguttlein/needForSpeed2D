#include "client_dibujador.h"
#include <SDL_image.h>
#include <cmath>
#include <algorithm>
#include <cstdio>

ClientDibujador::ClientDibujador(SDL_Renderer* r, int w, int h)
    : ren(r), winW(w), winH(h) {
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            SDL_Log("TTF_Init error: %s", TTF_GetError());
        }
    }
}

ClientDibujador::~ClientDibujador() {
    if (uiFont) { TTF_CloseFont(uiFont); uiFont = nullptr; }
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

bool ClientDibujador::setUIFont(const std::string& ttfPath, int size) {
    if (uiFont) { TTF_CloseFont(uiFont); uiFont = nullptr; }
    uiFont = TTF_OpenFont(ttfPath.c_str(), size);
    if (!uiFont) {
        SDL_Log("TTF_OpenFont('%s'): %s", ttfPath.c_str(), TTF_GetError());
        return false;
    }
    uiFontSize = size;
    return true;
}

static inline float norm360(float a) {
    while(a < 0.f)    a += 360.f;
    while(a >= 360.f) a -= 360.f;
    return a;
}

static inline float shortestsDelta(float fromDeg, float toDeg){
    float d = norm360(toDeg - fromDeg);
    if(d > 180.f) d -= 360.f;
    return d;
}

static inline float approachAngle(float curDeg, float targetDeg, float maxStepDeg){
    float d = shortestsDelta(curDeg, targetDeg);
    if (std::fabs(d) <= maxStepDeg) return norm360(targetDeg);
    return norm360(curDeg + (d > 0 ? maxStepDeg : -maxStepDeg));
}

int ClientDibujador::frameForAngle_(float angleDeg) const {
    const int total = atlasCols * atlasRows;
    const float step = 360.0f / static_cast<float>(total);
    float rel = clockwise ? (angleDeg - angle0) : (angle0 - angleDeg);
    rel = norm360(rel);
    const int idx = static_cast<int>(std::floor((rel + step * 0.5f) / step)) % total;
    return idx;
}

void ClientDibujador::updateCamera_(int px, int py) {
    const int marginX = winW / 4;
    const int marginY = winH / 4;

    int left   = camX + marginX;
    int right  = camX + winW - marginX;
    int top    = camY + marginY;
    int bottom = camY + winH - marginY;

    if (px < left)   camX -= (left - px);
    if (px > right)  camX += (px - right);
    if (py < top)    camY -= (top - py);
    if (py > bottom) camY += (py - bottom);

    if (mapW > 0 && mapH > 0) {
        camX = std::clamp(camX, 0, std::max(0, mapW - winW));
        camY = std::clamp(camY, 0, std::max(0, mapH - winH));
    } else {
        camX = std::max(0, camX);
        camY = std::max(0, camY);
    }
}

void ClientDibujador::renderFrame(int playerX, int playerY) {
    int px = playerX;
    int py = playerY;

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
        SDL_Rect d{ px - camX - cellW/2, py - camY - cellH/2, cellW, cellH };
        SDL_RenderCopy(ren, carTex, &s, &d);
    }

    SDL_RenderPresent(ren);
}

void ClientDibujador::renderAll(const std::vector<CarStateDTO>& cars, int selfId) {
    // Fondo
    SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
    SDL_RenderClear(ren);

    // Mapa
    if (mapTex) {
        SDL_Rect src{ camX, camY, winW, winH };
        SDL_Rect dst{ 0, 0, winW, winH };
        SDL_RenderCopy(ren, mapTex, &src, &dst);
    }

    // Autos
    for (const auto& carState : cars) {
        int px = static_cast<int>(carState.position.x * Constants::SCALE_METER_TO_PIXEL);
        int py = static_cast<int>(carState.position.y * Constants::SCALE_METER_TO_PIXEL);

        float angleRad = std::atan2(carState.angle.y, carState.angle.x);
        float currentAngleDeg = angleRad * 180.0f / static_cast<float>(M_PI);

        if (carState.car_id == selfId) {
            updateCamera_(px, py);
        }

        if (carTex && cellW > 0 && cellH > 0) {
            int frame = frameForAngle_(currentAngleDeg);
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

    hudPlayers_ = std::max(1, (int)cars.size());
    drawHUD_();

    SDL_RenderPresent(ren);
}

void ClientDibujador::drawPanel_(int x, int y, int w, int h, Uint8 a) {
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, 16, 16, 20, a);
    SDL_Rect r{ x, y, w, h };
    SDL_RenderFillRect(ren, &r);
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 50);
    SDL_RenderDrawRect(ren, &r);
}

void ClientDibujador::drawText_(const std::string& s, int x, int y, SDL_Color col, bool centerY) {
    if (!uiFont) return;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(uiFont, s.c_str(), col);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);

    SDL_Rect dst{ x, y, surf->w, surf->h };
    if (centerY) dst.y -= surf->h / 2;

    SDL_FreeSurface(surf);
    if (tex) {
        SDL_RenderCopy(ren, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }
}

void ClientDibujador::drawBadge_(int x, int y, const std::string& label,
                                 const std::string& value,
                                 SDL_Color labelCol, SDL_Color valueCol) {
    const int padX = 12;
    const int padY = 8;

    int lw = 0, lh = 0, vw = 0, vh = 0;
    if (uiFont) {
        TTF_SizeUTF8(uiFont, label.c_str(), &lw, &lh);
        TTF_SizeUTF8(uiFont, value.c_str(), &vw, &vh);
    }
    const int w = padX + lw + 8 + vw + padX;
    const int h = std::max(lh, vh) + padY * 2;

    drawPanel_(x, y, w, h, 150);

    int cy = y + h / 2;
    drawText_(label, x + padX, cy, labelCol, true);
    drawText_(value, x + padX + lw + 8, cy, valueCol, true);
}

void ClientDibujador::drawHUD_() {
    if (!uiFont) {
        return;
    }

    const int pad = 12;
    const int panelW = 600;
    const int panelH = 64;
    const int px = (winW - panelW) / 2;
    const int py = winH - panelH - pad;

    drawPanel_(px, py, panelW, panelH);

    {
        std::string label = "Speed: ";
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%d km/h", (int)std::round(hudSpeedKph_));
        drawBadge_(px + 12, py + 12, label, buf);
    }

    {
        std::string label = "♥: ";
        SDL_Color heartCol = { 240, 50, 60, 255 };
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%d/%d", hudHp_, hudMaxHp_);

        int lw=0, lh=0;
        TTF_SizeUTF8(uiFont, label.c_str(), &lw, &lh);
        int badgeX = px + 12 + 170;
        int badgeY = py + 12;

        int vw=0, vh=0;
        TTF_SizeUTF8(uiFont, buf, &vw, &vh);
        int w = 12 + lw + 8 + vw + 12;
        int h = std::max(lh, vh) + 16;
        drawPanel_(badgeX, badgeY, w, h, 150);

        int cy = badgeY + h / 2;
        drawText_(label, badgeX + 12, cy, heartCol, true);
        drawText_(buf,   badgeX + 12 + lw + 8, cy, SDL_Color{235,235,235,255}, true);
    }

    {
        char value[32];
        std::snprintf(value, sizeof(value), "%d/%d", std::max(1, hudPos_), std::max(1, hudPlayers_));
        drawBadge_(px + 12 + 170 + 120, py + 12, "Race: ", value);
    }

    {
        char value[32];
        std::snprintf(value, sizeof(value), "%d/%d", hudCP_, hudCPTotal_);
        int lw=0, lh=0, vw=0, vh=0;
        TTF_SizeUTF8(uiFont, "Checkpoints: ", &lw, &lh);
        TTF_SizeUTF8(uiFont, value, &vw, &vh);
        int bw = 12 + lw + 8 + vw + 12;
        int bx = px + panelW - bw - 12;
        int by = py + 12;
        drawBadge_(bx, by, "Checkpoints: ", value);
    }
}
