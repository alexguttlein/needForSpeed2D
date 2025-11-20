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
    if (checkpointTex) SDL_DestroyTexture(checkpointTex);
    if (hintTex) SDL_DestroyTexture(hintTex);
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

bool ClientDibujador::loadCheckpoint(const std::string& pathPng) {
    checkpointTex = loadTexture_(pathPng);
    return checkpointTex != nullptr;
}

bool ClientDibujador::loadHint(const std::string& pathPng) {
    hintTex = loadTexture_(pathPng);
    return hintTex != nullptr;
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

void ClientDibujador::setRaceFinished(bool finished, const std::vector<RaceStateDTO>& standings) {
    raceFinished_ = finished;
    finalStandings_ = standings;
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
    // if (raceFinished_) {
    //     renderResultsTable();
    //     return;
    // }

    if (!raceStarted_) {
        raceStarted_ = true;
        raceStartTicks_ = SDL_GetTicks();
    }

    SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
    SDL_RenderClear(ren);

    // Primero: encontrar mi auto y mover la cámara antes de dibujar nada
    for (const auto& carState : cars) {
        if (carState.car_id == selfId) {
            int px = static_cast<int>(carState.position.x * Constants::SCALE_METER_TO_PIXEL);
            int py = static_cast<int>(carState.position.y * Constants::SCALE_METER_TO_PIXEL);
            updateCamera_(px, py);
            hudHp_       = carState.health;
            hudSpeedKph_ = carState.speed * 3.6f; // m/s a km/h
            selfScreenX_ = px - camX;
            selfScreenY_ = py - camY;
            break;
        }
    }

    // Mapa con cámara actualizada
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

        if (carTex && cellW > 0 && cellH > 0) {
            int frame = frameForAngle_(currentAngleDeg);
            int col   = frame % atlasCols;
            int row   = frame / atlasCols;

            SDL_Rect s{ col * cellW, row * cellH, cellW, cellH };
            SDL_Rect d{
                px - camX - cellW / 2,
                py - camY - cellH / 2,
                cellW, cellH
            };
            SDL_RenderCopy(ren, carTex, &s, &d);
        }
    }

    drawCheckpoint_();
    drawHints_();

    hudPlayers_ = std::max(1, (int)cars.size());
    drawHUD_();
    drawMinimap_(cars, selfId);

    SDL_RenderPresent(ren);
}

void ClientDibujador::renderResultsTable() {
    renderResultsBackground_();

    if (!uiFont) {
        SDL_RenderPresent(ren);
        return;
    }

    const int margin = 40;

    SDL_Rect tableRect;
    tableRect.w = static_cast<int>(winW * 0.65f); 
    tableRect.h = static_cast<int>(winH * 0.6f);
    tableRect.x = margin;
    tableRect.y = (winH - tableRect.h) / 2;

    SDL_Rect upgradesRect;
    upgradesRect.w = static_cast<int>(winW * 0.22f);
    upgradesRect.h = tableRect.h;
    upgradesRect.x = winW - upgradesRect.w - margin;
    upgradesRect.y = tableRect.y;

    renderResultsTablePanel_(tableRect);
    renderResultsUpgradesPanel_(upgradesRect);
    SDL_RenderPresent(ren);
}

void ClientDibujador::renderResultsBackground_() {
    if (mapTex) {
        SDL_Rect src{ 0, 0, mapW, mapH };
        SDL_Rect dst{ 0, 0, winW, winH };

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
        Uint8 oldAlpha = 255;
        SDL_GetTextureAlphaMod(mapTex, &oldAlpha);
        SDL_SetTextureAlphaMod(mapTex, 80);
        SDL_RenderCopy(ren, mapTex, &src, &dst);
        SDL_SetTextureAlphaMod(mapTex, oldAlpha);
    } else {
        SDL_SetRenderDrawColor(ren, 10, 10, 15, 255);
        SDL_RenderClear(ren);
    }
}

void ClientDibujador::renderResultsTablePanel_(const SDL_Rect& tableRect) {

    drawPanel_(tableRect.x, tableRect.y, tableRect.w, tableRect.h, 200);

    std::string title = "Resultados de la carrera";
    int titleX = tableRect.x + (tableRect.w / 2) - 140;
    int titleY = tableRect.y + 20;
    drawText_(title, titleX, titleY, {255, 255, 255, 255}, false);

    int startY = tableRect.y + 70;
    int lineH  = uiFontSize + 6;

    std::vector<RaceStateDTO> ordered = finalStandings_;
    std::sort(ordered.begin(), ordered.end(),
              [](const RaceStateDTO& a, const RaceStateDTO& b) {
                  return a.finishPosition < b.finishPosition;
              });

    for (size_t i = 0; i < ordered.size(); ++i) {
        const auto& rs = ordered[i];
        int   pos  = rs.finishPosition;
        float time = rs.finishTimeSeconds;

        char buffer[128];
        if (time >= 0.0f) {
            std::snprintf(buffer, sizeof(buffer),
                          "%d) Jugador %d - %.2f s",
                          pos, rs.playerId, time);
        } else {
            std::snprintf(buffer, sizeof(buffer),
                          "%d) Jugador %d - DNF",
                          pos, rs.playerId);
        }
        int textX = tableRect.x + 40;
        int textY = startY + static_cast<int>(i) * lineH;

        if (textY + lineH > tableRect.y + tableRect.h - 20) {
            break;
        }

        drawText_(buffer, textX, textY, {230, 230, 230, 255}, false);
    }
}

void ClientDibujador::renderResultsUpgradesPanel_(const SDL_Rect& panelRect) {
    drawPanel_(panelRect.x, panelRect.y, panelRect.w, panelRect.h, 180);

    drawText_("Mejoras",
              panelRect.x + 20,
              panelRect.y + 16,
              {255, 255, 255, 255},
              false);

    int y  = panelRect.y + 60;
    int dy = uiFontSize + 14;

    drawText_("[Q] + Vida",
              panelRect.x + 20,
              y,
              {220, 220, 220, 255},
              false);
    y += dy;

    drawText_("[E] + Aceleracion",
              panelRect.x + 20,
              y,
              {220, 220, 220, 255},
              false);
    y += dy;

    drawText_("[V] + Velocidad",
              panelRect.x + 20,
              y,
              {220, 220, 220, 255},
              false);
    y += dy;

    drawText_("[C] + Control",
              panelRect.x + 20,
              y,
              {220, 220, 220, 255},
              false);
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

    const int pad    = 0;
    const int leftPx     = pad;
    const int leftPy     = pad;

    drawHudSpeed_(leftPx, leftPy);
    drawHudHealth_(leftPx, leftPy);

    const int rightPanelW = 300;
    const int rightPx     = winW - rightPanelW - pad;
    const int rightPy     = pad;

    drawHudRace_(rightPx, rightPy);
    drawHudTime_(rightPx, rightPy, rightPanelW);
}


void ClientDibujador::drawHudSpeed_(int panelX, int panelY) {
    std::string label = "Speed: ";
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%d km/h", (int)std::round(hudSpeedKph_));

    int x = panelX + 12;
    int y = panelY + 12;

    drawBadge_(x, y, label, buf);
}

void ClientDibujador::drawHudHealth_(int panelX, int panelY) {
    std::string label = "♥: ";
    SDL_Color heartCol   = { 240,  50,  60, 255 };
    SDL_Color valueColor = { 235, 235, 235, 255 };

    char buf[64];
    std::snprintf(buf, sizeof(buf), "%d/%d", hudHp_, hudMaxHp_);

    int baseX = panelX + 12 + 170;
    int baseY = panelY + 12;

    int lw = 0, lh = 0;
    int vw = 0, vh = 0;

    if (uiFont) {
        TTF_SizeUTF8(uiFont, label.c_str(), &lw, &lh);
        TTF_SizeUTF8(uiFont, buf,   &vw, &vh);
    }

    const int padX = 12;
    const int padY = 8;

    int w = padX + lw + 8 + vw + padX;
    int h = std::max(lh, vh) + padY * 2;

    drawPanel_(baseX, baseY, w, h, 150);

    int cy = baseY + h / 2;
    drawText_(label, baseX + padX, cy, heartCol, true);
    drawText_(buf,   baseX + padX + lw + 8, cy, valueColor, true);
}

void ClientDibujador::drawHudRace_(int panelX, int panelY) {
    char value[32];
    std::snprintf(
        value,
        sizeof(value),
        "%d/%d",
        std::max(1, hudPos_),
        std::max(1, hudPlayers_)
    );

    int x = panelX + 12;
    int y = panelY + 12;

    drawBadge_(x, y, "Race: ", value);
}

void ClientDibujador::updateRaceState(const RaceStateDTO& raceState) {
    hudNextCheckpoint_.x = raceState.nextCheckpoint.x * Constants::SCALE_METER_TO_PIXEL;
    hudNextCheckpoint_.y = raceState.nextCheckpoint.y * Constants::SCALE_METER_TO_PIXEL;

    hudHints_.clear();
    hudHints_.reserve(raceState.currentHints.size());
    for (const auto& h : raceState.currentHints) {
        Vector2D<float> p;
        p.x = h.x * Constants::SCALE_METER_TO_PIXEL;
        p.y = h.y * Constants::SCALE_METER_TO_PIXEL;
        hudHints_.push_back(p);
    }
}

void ClientDibujador::drawHudTime_(int panelX, int panelY, int panelW) {
    Uint32 now = SDL_GetTicks();
    Uint32 elapsedMs = raceStarted_ ? (now - raceStartTicks_) : 0;

    Uint32 totalSeconds = elapsedMs / 1000;
    Uint32 minutes = totalSeconds / 60;
    Uint32 seconds = totalSeconds % 60;

    char value[32];
    std::snprintf(value, sizeof(value), "%02u:%02u", minutes, seconds);

    int lw = 0, lh = 0, vw = 0, vh = 0;
    if (uiFont) {
        TTF_SizeUTF8(uiFont, "Time: ", &lw, &lh);
        TTF_SizeUTF8(uiFont, value,   &vw, &vh);
    }

    int bw = 12 + lw + 8 + vw + 12;
    int bx = panelX + panelW - bw - 12;
    int by = panelY + 12;

    drawBadge_(bx, by, "Time: ", value);
}

void ClientDibujador::drawCheckpoint_() {
    int screenX = static_cast<int>(hudNextCheckpoint_.x) - camX;
    int screenY = static_cast<int>(hudNextCheckpoint_.y) - camY;

    if (screenX < -20 || screenX > winW + 20 ||
        screenY < -20 || screenY > winH + 20) {
        return;
    }

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    if (checkpointTex) {
        int texW = 0, texH = 0;
        SDL_QueryTexture(checkpointTex, nullptr, nullptr, &texW, &texH);
        float scale = 1.0f;
        if (texW > checkpointSizePx || texH > checkpointSizePx) {
            float sx = checkpointSizePx / static_cast<float>(texW);
            float sy = checkpointSizePx / static_cast<float>(texH);
            scale = std::min(sx, sy);
        }

        int dstW = static_cast<int>(texW * scale);
        int dstH = static_cast<int>(texH * scale);
        SDL_Rect dst{ screenX - dstW / 2, screenY - dstH / 2, dstW, dstH };
        SDL_RenderCopy(ren, checkpointTex, nullptr, &dst);
    } else {
        SDL_SetRenderDrawColor(ren, 250, 215, 70, 220);
        SDL_Rect r{ screenX - 12, screenY - 12, 24, 24 };
        SDL_RenderFillRect(ren, &r);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &r);
    }
}

void ClientDibujador::drawHints_() {
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    for (const auto& h : hudHints_) {
        int screenX = static_cast<int>(h.x) - camX;
        int screenY = static_cast<int>(h.y) - camY;

        if (screenX < -10 || screenX > winW + 10 ||
            screenY < -10 || screenY > winH + 10) {
            continue;
        }

        if (hintTex) {
            int texW = 0, texH = 0;
            SDL_QueryTexture(hintTex, nullptr, nullptr, &texW, &texH);

            float scale = 1.0f;
            if (texW > hintSizePx || texH > hintSizePx) {
                float sx = hintSizePx / static_cast<float>(texW);
                float sy = hintSizePx / static_cast<float>(texH);
                scale = std::min(sx, sy);
            }

            int dstW = static_cast<int>(texW * scale);
            int dstH = static_cast<int>(texH * scale);
            SDL_Rect dst{ screenX - dstW / 2, screenY - dstH / 2, dstW, dstH };

            float dx = hudNextCheckpoint_.x - h.x;
            float dy = hudNextCheckpoint_.y - h.y;
            float angleRad = std::atan2(dy, dx);
            float angleDeg = angleRad * 180.0f / static_cast<float>(M_PI);

            float renderAngle = angleDeg + 270.0f;

            SDL_RenderCopyEx(ren, hintTex, nullptr, &dst, renderAngle, nullptr, SDL_FLIP_NONE);
        } else {
            SDL_SetRenderDrawColor(ren, 80, 200, 250, 230);
            SDL_Rect r{ screenX - 4, screenY - 4, 8, 8 };
            SDL_RenderFillRect(ren, &r);
        }
    }
}

void ClientDibujador::drawMinimap_(const std::vector<CarStateDTO>& cars, int selfId) {
    if (!mapTex || mapW <= 0 || mapH <= 0) {
        return;
    }

    const CarStateDTO* selfCar = nullptr;
    for (const auto& c : cars) {
        if (c.car_id == selfId) {
            selfCar = &c;
            break;
        }
    }
    if (!selfCar) return;

    const int pad      = 0;
    const int miniSize = 110;
    const int miniX    = pad;
    const int miniY    = winH - pad - miniSize;

    const int viewWDesired = 1100;
    const int viewHDesired = 1100;

    float selfX = selfCar->position.x * Constants::SCALE_METER_TO_PIXEL;
    float selfY = selfCar->position.y * Constants::SCALE_METER_TO_PIXEL;

    int srcW = std::min(viewWDesired, mapW);
    int srcH = std::min(viewHDesired, mapH);

    int srcX = static_cast<int>(selfX) - srcW / 2;
    int srcY = static_cast<int>(selfY) - srcH / 2;

    srcX = std::clamp(srcX, 0, mapW - srcW);
    srcY = std::clamp(srcY, 0, mapH - srcH);

    SDL_Rect src{ srcX, srcY, srcW, srcH };
    SDL_Rect dst{ miniX, miniY, miniSize, miniSize };

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, 10, 10, 10, 180);
    SDL_RenderFillRect(ren, &dst);

    Uint8 oldAlpha = 255;
    SDL_GetTextureAlphaMod(mapTex, &oldAlpha);
    SDL_SetTextureAlphaMod(mapTex, 255);

    SDL_RenderCopy(ren, mapTex, &src, &dst);

    SDL_SetTextureAlphaMod(mapTex, oldAlpha);


    SDL_SetRenderDrawColor(ren, 255, 255, 255, 80);
    SDL_RenderDrawRect(ren, &dst);

    for (const auto& car : cars) {
        int worldX = static_cast<int>(car.position.x * Constants::SCALE_METER_TO_PIXEL);
        int worldY = static_cast<int>(car.position.y * Constants::SCALE_METER_TO_PIXEL);

        float nx = (worldX - srcX) / static_cast<float>(srcW);
        float ny = (worldY - srcY) / static_cast<float>(srcH);

        if (nx < 0.0f || nx > 1.0f || ny < 0.0f || ny > 1.0f) {
            continue;
        }

        int dotX = dst.x + static_cast<int>(nx * dst.w + 0.5f);
        int dotY = dst.y + static_cast<int>(ny * dst.h + 0.5f);

        if (car.car_id == selfId) {
            SDL_SetRenderDrawColor(ren, 220, 40, 40, 255);
        } else {
            SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        }

        SDL_Rect r{ dotX - 2, dotY - 2, 5, 5 };
        SDL_RenderFillRect(ren, &r);
    }
}

