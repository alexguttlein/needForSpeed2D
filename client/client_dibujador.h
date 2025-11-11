#ifndef TP_TALLER_G7_CLIENT_DIBUJADOR_H
#define TP_TALLER_G7_CLIENT_DIBUJADOR_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "../common/carStateDTO.h"
#include "../common/constants.h"

class ClientDibujador {
public:
    ClientDibujador(SDL_Renderer* r, int winW, int winH);
    ~ClientDibujador();

    // Carga de recursos visuales
    bool loadMap(const std::string& pathPng);
    bool loadCarAtlas(const std::string& pathPng, int cols = 8, int rows = 2,
                      float angle0Deg = 0.f, bool clockwise = true);

    // Fuente UI (llamalo una vez al iniciar)
    bool setUIFont(const std::string& ttfPath, int size = 16);

    // Render “single” (lo dejé intacto para tus pruebas)
    void renderFrame(int playerX, int playerY);
    void setFacingDeg(float deg) { facingDeg = deg; }

    // Render principal
    void renderAll(const std::vector<CarStateDTO>& cars, int selfId);

    // ------- HUD (valores que hoy simulamos) -------
    void setHUDPosition(int pos)            { hudPos_ = pos; }
    void setHUDSpeedKph(float kph)          { hudSpeedKph_ = kph; }
    void setHUDHp(int hp, int maxHp)        { hudHp_ = hp; hudMaxHp_ = maxHp; }
    void setHUDCheckpoints(int cur, int tot){ hudCP_ = cur; hudCPTotal_ = tot; }

private:
    // Internas
    SDL_Texture* loadTexture_(const std::string& path);
    int frameForAngle_(float angleDeg) const;
    void updateCamera_(int playerX, int playerY);

    // ---------- HUD helpers (sin barras) ----------
    void drawHUD_();
    void drawPanel_(int x, int y, int w, int h, Uint8 a = 160);
    void drawBadge_(int x, int y, const std::string& label,
                    const std::string& value,
                    SDL_Color labelCol = {180, 180, 190, 255},
                    SDL_Color valueCol = {235, 235, 235, 255});
    void drawText_(const std::string& s, int x, int y, SDL_Color col, bool centerY = true);

    // Renderer / ventana
    SDL_Renderer* ren;
    int winW, winH;

    // Mapa
    SDL_Texture* mapTex = nullptr;
    int mapW = 0, mapH = 0;

    // Autos (atlas)
    SDL_Texture* carTex = nullptr;
    int atlasCols = 8, atlasRows = 2;
    int cellW = 0, cellH = 0;
    float angle0 = 0.0f;
    bool clockwise = true;

    // Cámara
    int camX = 0, camY = 0;

    // Estado “single player”
    float facingDeg = 0.0f;
    int lastX = -1, lastY = -1;

    // ---------- Estado del HUD ----------
    int   hudPos_       = 1;      // posición P
    int   hudPlayers_   = 1;      // N jugadores (se actualiza desde cars.size())
    int   hudHp_        = 82;     // vida actual
    int   hudMaxHp_     = 100;    // vida máxima
    float hudSpeedKph_  = 128.f;  // velocidad simulada (km/h)
    int   hudCP_        = 3;      // checkpoints actuales
    int   hudCPTotal_   = 30;     // checkpoints totales

    // Fuente UI
    TTF_Font* uiFont = nullptr;
    int uiFontSize = 16;
};

#endif // TP_TALLER_G7_CLIENT_DIBUJADOR_H
