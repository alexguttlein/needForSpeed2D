#include "client_dibujador.h"
#include <SDL_image.h>
#include <cmath>
#include <algorithm>
#include <cstdio>

static inline float norm360(float a);

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

    for (auto& [id, atlas] : carAtlases_) {
        if (atlas.tex) SDL_DestroyTexture(atlas.tex);
        atlas.tex = nullptr;
    }
    carAtlases_.clear();

    if (mapTex) SDL_DestroyTexture(mapTex);
    if (checkpointTex) SDL_DestroyTexture(checkpointTex);
    if (hintTex) SDL_DestroyTexture(hintTex);
}


SDL_Texture* ClientDibujador::loadTexture_(const std::string& path) {

    SDL_Surface* surf = IMG_Load(path.c_str());
    if (!surf) return nullptr;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);

    // 🔥 Esto es lo que habilita el canal alfa del PNG
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

    return tex;
}

bool ClientDibujador::loadMap(const std::string& pathPng, const std::string& pathPngOver) {

    // Cargar mapa base
    mapTex = loadTexture_(pathPng);
    if (!mapTex) return false;

    mapOverTex = loadTexture_(pathPngOver);
    if (!mapTex) return false;

    SDL_QueryTexture(mapTex, nullptr, nullptr, &mapW, &mapH);

    // Cargar capa superior (puentes)
    if (!pathPngOver.empty()) {
        mapOverTex = loadTexture_(pathPngOver);
        // mapOverTex puede ser null si no existe,
        // no hacemos return false para que sea opcional.
    }

    return true;
}

bool ClientDibujador::loadCarAtlasForId(int carTypeId, const std::string& path,
                                       int cols, int rows, float angle0Deg, bool cw) {
    CarAtlas atlas;
    atlas.tex = loadTexture_(path);
    if (!atlas.tex) {
        std::fprintf(stderr, "No pude cargar auto %d: %s (IMG err=%s)\n",
                     carTypeId, path.c_str(), IMG_GetError());
        return false;
    }

    atlas.cols = cols;
    atlas.rows = rows;
    atlas.angle0 = angle0Deg;
    atlas.clockwise = cw;

    int w, h;
    SDL_QueryTexture(atlas.tex, nullptr, nullptr, &w, &h);
    atlas.cellW = w / cols;
    atlas.cellH = h / rows;

    carAtlases_[carTypeId] = atlas;
    std::fprintf(stderr, "[DEBUG] Atlas cargado para carTypeId=%d: cellW=%d cellH=%d\n", 
                carTypeId, atlas.cellW, atlas.cellH);

    // IMPORTANTÍSIMO: si es el default (0), también seteá los miembros viejos
    if (carTypeId == 0) {
        carTex = atlas.tex;
        atlasCols = cols;
        atlasRows = rows;
        cellW = atlas.cellW;
        cellH = atlas.cellH;
        angle0 = angle0Deg;
    }

    return true;
}

const CarAtlas* ClientDibujador::atlasFor(int carTypeId) const {
    auto it = carAtlases_.find(carTypeId);
    if (it != carAtlases_.end()) return &it->second;

    // fallback: default
    it = carAtlases_.find(0);
    if (it != carAtlases_.end()) return &it->second;

    return nullptr;
}

int ClientDibujador::frameForAngle_(float angleDeg, const CarAtlas& atlas) const {
    const int total = atlas.cols * atlas.rows;
    const float step = 360.0f / float(total);

    float rel = atlas.clockwise ? (angleDeg - atlas.angle0) : (atlas.angle0 - angleDeg);
    rel = norm360(rel);

    int idx = int(std::floor((rel + step * 0.5f) / step)) % total;
    return idx;
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
    if (finished) {
        if (!raceFinished_) {
            raceFinished_ = true;
            finalStandings_ = standings;
            resultsStartTicks_ = SDL_GetTicks();
        } else {
            finalStandings_ = standings;
        }
    } else {
        raceFinished_ = false;
        finalStandings_.clear();
        resultsStartTicks_ = 0;
    }
}

void ClientDibujador::renderAll(const std::vector<CarStateDTO>& cars, int selfId, std::string timeLeftRace) {
    if (gameFinished_) {
        renderGameOver_();
        return;
    }
    
    if (raceFinished_) {
        Uint32 now = SDL_GetTicks();
        const Uint32 SHOW_RESULTS_MS = 15000;

        if (now - resultsStartTicks_ < SHOW_RESULTS_MS) {
            currentCheckpoint = 0;
            renderResultsTable();
            return;
        } else {
            raceFinished_ = false;
            finalStandings_.clear();
            resultsStartTicks_ = 0;

            raceStarted_ = false;
            raceStartTicks_ = 0;
            playerFinishedRace_ = false;
        }
    }

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
        int px = int(carState.position.x * Constants::SCALE_METER_TO_PIXEL);
        int py = int(carState.position.y * Constants::SCALE_METER_TO_PIXEL);

        float angleRad = std::atan2(carState.angle.y, carState.angle.x);
        float currentAngleDeg = angleRad * 180.0f / float(M_PI);

        // Usar el car_type_id del snapshot, con fallback si es 0
        int typeId = carState.car_type_id;
        if (typeId == 0) {
            // Fallback: usar car_id + 1 para asignar un tipo visual
            typeId = (carState.car_id % 7) + 1;
            static bool warned = false;
            if (!warned) {
                std::fprintf(stderr, "[WARN] car_type_id es 0, usando fallback: car_id=%d -> typeId=%d\n", 
                            carState.car_id, typeId);
                warned = true;
            }
        }

        const CarAtlas* atlas = atlasFor(typeId);

        if (atlas && atlas->tex && atlas->cellW > 0 && atlas->cellH > 0) {
            int frame = frameForAngle_(currentAngleDeg, *atlas);
            int col = frame % atlas->cols;
            int row = frame / atlas->cols;

            SDL_Rect s{ col * atlas->cellW, row * atlas->cellH, atlas->cellW, atlas->cellH };
            SDL_Rect d{ px - camX - atlas->cellW/2, py - camY - atlas->cellH/2,
                        atlas->cellW, atlas->cellH };

            SDL_RenderCopy(ren, atlas->tex, &s, &d);
        } else {
            std::fprintf(stderr, "[ERROR] No se encontró atlas para typeId=%d (car_id=%d)\n", 
                        typeId, carState.car_id);
        }
    }

    if (mapOverTex) {
        SDL_Rect src{ camX, camY, winW, winH };
        SDL_Rect dst{ 0, 0, winW, winH };

        SDL_SetTextureBlendMode(mapOverTex, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(mapOverTex, 255);   // Cambiá este valor a gusto (0-255)

        SDL_RenderCopy(ren, mapOverTex, &src, &dst);
    }

    drawCheckpoint_();
    drawHints_();

    drawHUD_(timeLeftRace);
    drawMinimap_(cars, selfId);
    
    // Mostrar popup si el jugador terminó pero la carrera no ha finalizado
    if (playerFinishedRace_ && !raceFinished_) {
        renderWaitingForPlayers_();
    }
    
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
    
    if (showUpgradePopup_) {
        renderUpgradePopup_();
    }
    
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
        std::string playerName = rs.playerName;

        char buffer[128];
        if (time >= 0.0f) {
            std::snprintf(buffer, sizeof(buffer),
                          "%d) %s - %.2f s",
                          pos, playerName.c_str(), time);
        } else {
            std::snprintf(buffer, sizeof(buffer),
                          "%d) %s - DNF",
                          pos, playerName.c_str());
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

    SDL_Color titleColor{255, 255, 255, 255};
    SDL_Color nameColor{230, 230, 230, 255};
    SDL_Color costColor{190, 190, 190, 255};

    const int marginX   = 18;
    const int titleY    = panelRect.y + 18;
    const int startY    = panelRect.y + 55;
    const int lineGap   = uiFontSize + 4;
    const int blockGap  = uiFontSize + 10;

    int x = panelRect.x + marginX;
    int y = startY;

    // Título
    drawText_("Mejoras", x, titleY, titleColor, false);

    drawText_("[1] + Escudo", x, y, nameColor, false);
    y += lineGap;

    drawText_("Costo: 8 s", x + 10, y, costColor, false);
    y += blockGap;

    drawText_("[2] + Aceleracion", x, y, nameColor, false);
    y += lineGap;

    drawText_("Costo: 6 s", x + 10, y, costColor, false);
    y += blockGap;

    drawText_("[3] + Control", x, y, nameColor, false);
    y += lineGap;

    drawText_("Costo: 10 s", x + 10, y, costColor, false);
    y += blockGap;

    drawText_("[4] + MAX Velocidad", x, y, nameColor, false);
    y += lineGap;

    drawText_("Costo: 12 s", x + 10, y, costColor, false);
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

void ClientDibujador::drawHUD_(std::string timeLeftRace) {
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
    drawHudTime_(rightPx, rightPy, rightPanelW, timeLeftRace);
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
        currentCheckpoint,
        numberOfCheckpoints
    );

    int x = panelX + 12;
    int y = panelY + 12;

    drawBadge_(x, y, "Checkpoints: ", value);
}

void ClientDibujador::updateRaceState(const RaceStateDTO& raceState) {
    Vector2D<float> newCheckpoint;
    newCheckpoint.x = raceState.nextCheckpoint.x * Constants::SCALE_METER_TO_PIXEL;
    newCheckpoint.y = raceState.nextCheckpoint.y * Constants::SCALE_METER_TO_PIXEL;
    
    // Si el checkpoint cambió, incrementar el contador
    if ((hudNextCheckpoint_.x != newCheckpoint.x || hudNextCheckpoint_.y != newCheckpoint.y) &&
        hudNextCheckpoint_.x != 0.0f && hudNextCheckpoint_.y != 0.0f) {
        currentCheckpoint++;
    }
    
    hudNextCheckpoint_ = newCheckpoint;

    hudHints_.clear();
    hudHints_.reserve(raceState.currentHints.size());
    for (const auto& h : raceState.currentHints) {
        Vector2D<float> p;
        p.x = h.x * Constants::SCALE_METER_TO_PIXEL;
        p.y = h.y * Constants::SCALE_METER_TO_PIXEL;
        hudHints_.push_back(p);
    }
    
    playerFinishedRace_ = raceState.hasFinished;
}

void ClientDibujador::drawHudTime_(int panelX, int panelY, int panelW, std::string timeLeftRace) {
    // Uint32 now = SDL_GetTicks();
    // Uint32 elapsedMs = raceStarted_ ? (now - raceStartTicks_) : 0;

    // Uint32 totalSeconds = elapsedMs / 1000;
    // Uint32 minutes = totalSeconds / 60;
    // Uint32 seconds = totalSeconds % 60;

    char value[32];
    std::snprintf(value, sizeof(value), "%s", timeLeftRace.c_str());

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
    if (playerFinishedRace_) {
        return;
    }
    
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

void ClientDibujador::showUpgradePopup(int upgradeId) {
    showUpgradePopup_ = true;
    selectedUpgradeId_ = upgradeId;
    upgradePopupStartTicks_ = SDL_GetTicks();
}

void ClientDibujador::hideUpgradePopup() {
    showUpgradePopup_ = false;
    selectedUpgradeId_ = 0;
}

std::string ClientDibujador::getUpgradeName_(int upgradeId) const {
    switch (upgradeId) {
        case 1: return "Escudo";
        case 2: return "Aceleracion";
        case 3: return "Control";
        case 4: return "Velocidad Maxima";
        default: return "Mejora Desconocida";
    }
}

std::string ClientDibujador::getUpgradeDescription_(int upgradeId) const {
    switch (upgradeId) {
        case 1: return "proporciona un escudo que absorbe daño";
        case 2: return "Mejora la aceleracion del vehiculo";
        case 3: return "Mejora el manejo del vehiculo";
        case 4: return "Aumenta la velocidad maxima";
        default: return "";
    }
}

void ClientDibujador::renderUpgradePopup_() {
    if (!uiFont) return;

    const int popupW = 400;
    const int popupH = 200;
    const int popupX = (winW - popupW) / 2;
    const int popupY = (winH - popupH) / 2;

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 180);
    SDL_Rect fullScreen{0, 0, winW, winH};
    SDL_RenderFillRect(ren, &fullScreen);

    SDL_SetRenderDrawColor(ren, 30, 30, 40, 240);
    SDL_Rect popup{popupX, popupY, popupW, popupH};
    SDL_RenderFillRect(ren, &popup);

    SDL_SetRenderDrawColor(ren, 100, 200, 255, 255);
    SDL_RenderDrawRect(ren, &popup);
    
    SDL_Rect innerBorder{popupX + 2, popupY + 2, popupW - 4, popupH - 4};
    SDL_RenderDrawRect(ren, &innerBorder);

    std::string title = "MEJORA SELECCIONADA";
    drawText_(title, popupX + popupW / 2 - 80, popupY + 30, {100, 200, 255, 255}, true);

    std::string upgradeName = getUpgradeName_(selectedUpgradeId_);
    drawText_(upgradeName, popupX + popupW / 2 - 50, popupY + 80, {255, 255, 100, 255}, true);

    std::string description = getUpgradeDescription_(selectedUpgradeId_);
    drawText_(description, popupX + popupW / 2 - 120, popupY + 120, {200, 200, 200, 255}, true);

    std::string waitMsg = "Esperando proxima carrera...";
    drawText_(waitMsg, popupX + popupW / 2 - 100, popupY + 160, {150, 150, 150, 255}, true);
    
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);
}

void ClientDibujador::setGameFinished(bool finished, const std::vector<PlayerTime>& leaderboard) {
    gameFinished_ = finished;
    if (finished) {
        finalLeaderboard_ = leaderboard;
    } else {
        finalLeaderboard_.clear();
    }
}

void ClientDibujador::renderGameOver_() {
    if (!uiFont) {
        SDL_RenderPresent(ren);
        return;
    }

    // Fondo oscuro
    SDL_SetRenderDrawColor(ren, 10, 10, 15, 255);
    SDL_RenderClear(ren);

    // Panel principal centrado
    const int panelW = 600;
    const int panelH = 500;
    const int panelX = (winW - panelW) / 2;
    const int panelY = (winH - panelH) / 2 - 30;

    // Fondo del panel
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    drawPanel_(panelX, panelY, panelW, panelH, 230);

    // Borde dorado
    SDL_SetRenderDrawColor(ren, 255, 215, 0, 255);
    SDL_Rect border{panelX, panelY, panelW, panelH};
    SDL_RenderDrawRect(ren, &border);
    SDL_Rect innerBorder{panelX + 2, panelY + 2, panelW - 4, panelH - 4};
    SDL_RenderDrawRect(ren, &innerBorder);

    // Título "JUEGO FINALIZADO"
    std::string title = "JUEGO FINALIZADO";
    int titleX = panelX + panelW / 2 - 90;
    int titleY = panelY + 30;
    drawText_(title, titleX, titleY, {255, 215, 0, 255}, true);

    // Subtítulo
    std::string subtitle = "Clasificacion Final";
    int subtitleX = panelX + panelW / 2 - 80;
    int subtitleY = panelY + 70;
    drawText_(subtitle, subtitleX, subtitleY, {200, 200, 200, 255}, true);

    // Leaderboard
    std::vector<PlayerTime> sortedLeaderboard = finalLeaderboard_;
    std::sort(sortedLeaderboard.begin(), sortedLeaderboard.end(),
              [](const PlayerTime& a, const PlayerTime& b) {
                  return a.finishTime < b.finishTime;
              });

    int startY = panelY + 120;
    int lineHeight = 40;
    int position = 1;

    for (const auto& entry : sortedLeaderboard) {
        if (position > 8) break; // Máximo 8 posiciones

        int y = startY + (position - 1) * lineHeight;

        // Posición
        std::string posStr = std::to_string(position) + ".";
        SDL_Color posColor = {255, 255, 255, 255};
        if (position == 1) posColor = {255, 215, 0, 255}; // Oro
        else if (position == 2) posColor = {192, 192, 192, 255}; // Plata
        else if (position == 3) posColor = {205, 127, 50, 255}; // Bronce
        
        drawText_(posStr, panelX + 50, y, posColor, true);

        // Player Name
        std::string playerStr = entry.playerName;
        drawText_(playerStr, panelX + 120, y, {220, 220, 220, 255}, true);

        // Tiempo
        int minutes = static_cast<int>(entry.finishTime) / 60;
        int seconds = static_cast<int>(entry.finishTime) % 60;
        int millis = static_cast<int>((entry.finishTime - static_cast<int>(entry.finishTime)) * 1000);
        
        char timeBuffer[32];
        std::snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d.%03d", minutes, seconds, millis);
        std::string timeStr(timeBuffer);
        
        drawText_(timeStr, panelX + panelW - 150, y, {100, 200, 255, 255}, true);

        position++;
    }

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);
    SDL_RenderPresent(ren);
}

void ClientDibujador::renderWaitingForPlayers_() {
    if (!uiFont) return;

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    int panelW = 500;
    int panelH = 180;
    int panelX = (winW - panelW) / 2;
    int panelY = (winH - panelH) / 2;

    SDL_Rect panelRect = {panelX, panelY, panelW, panelH};
    SDL_SetRenderDrawColor(ren, 40, 40, 50, 230);
    SDL_RenderFillRect(ren, &panelRect);

    SDL_SetRenderDrawColor(ren, 255, 215, 0, 255);
    for (int i = 0; i < 3; ++i) {
        SDL_Rect borderRect = {panelX - i, panelY - i, panelW + 2 * i, panelH + 2 * i};
        SDL_RenderDrawRect(ren, &borderRect);
    }

    std::string title = "CARRERA COMPLETADA!";
    int titleX = panelX + panelW / 2 - 120;
    int titleY = panelY + 30;
    drawText_(title, titleX, titleY, {100, 255, 100, 255}, true);

    std::string waitMsg = "Esperando a los demas jugadores...";
    int msgX = panelX + panelW / 2 - 140;
    int msgY = panelY + 90;
    drawText_(waitMsg, msgX, msgY, {220, 220, 220, 255}, true);

    Uint32 now = SDL_GetTicks();
    int dotCount = (now / 500) % 4;
    std::string dots(dotCount, '.');
    int dotsX = panelX + panelW / 2 - 10;
    int dotsY = panelY + 125;
    drawText_(dots, dotsX, dotsY, {180, 180, 180, 255}, true);

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);
}


void ClientDibujador::setCheatCallback(std::function<void(const std::string&)> callback) {
    cheatCallback = std::move(callback);

    cheatDetector.setCheatCallback([this](const std::string& cheatCode) {
        if (cheatCallback) {
            cheatCallback(cheatCode);
        }
    });
}


void ClientDibujador::processKeyForCheat(SDL_Keycode key) {
    cheatDetector.processKeyPress(static_cast<SDL_KeyCode>(key));
}

