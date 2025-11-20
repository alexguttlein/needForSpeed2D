#ifndef TP_TALLER_G7_CLIENT_DIBUJADOR_H
#define TP_TALLER_G7_CLIENT_DIBUJADOR_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "../common/carStateDTO.h"
#include "../common/constants.h"
#include "../common/raceStateDTO.h"

class ClientDibujador {
public:
    ClientDibujador(SDL_Renderer* r, int winW, int winH);
    ~ClientDibujador();

    bool loadMap(const std::string& pathPng);
    bool loadCarAtlas(const std::string& pathPng, int cols = 8, int rows = 2,
                      float angle0Deg = 0.f, bool clockwise = true);

    bool setUIFont(const std::string& ttfPath, int size = 16);

    void renderFrame(int playerX, int playerY);
    void setFacingDeg(float deg) { facingDeg = deg; }

    void renderAll(const std::vector<CarStateDTO>& cars, int selfId);
    void updateRaceState(const RaceStateDTO& raceState);

    void setHUDPosition(int pos)            { hudPos_ = pos; }
    void setHUDSpeedKph(float kph)          { hudSpeedKph_ = kph; }
    void setHUDHp(int hp, int maxHp)        { hudHp_ = hp; hudMaxHp_ = maxHp; }

    bool loadCheckpoint(const std::string& pathPng);
    bool loadHint(const std::string& pathPng);

    void setRaceFinished(bool finished, const std::vector<RaceStateDTO>& standings);

    Vector2D<float> hudNextCheckpoint_{};               
    std::vector<Vector2D<float>> hudHints_{};

private:
    SDL_Texture* loadTexture_(const std::string& path);
    int frameForAngle_(float angleDeg) const;
    void updateCamera_(int playerX, int playerY);

    void drawHUD_();
    void drawHudSpeed_(int panelX, int panelY);
    void drawHudHealth_(int panelX, int panelY);
    void drawHudRace_(int panelX, int panelY);
    void drawHudTime_(int panelX, int panelY, int panelW);
    void drawMinimap_(const std::vector<CarStateDTO>& cars, int selfId);

    void drawPanel_(int x, int y, int w, int h, Uint8 a = 160);
    void drawBadge_(int x, int y, const std::string& label,
                    const std::string& value,
                    SDL_Color labelCol = {180, 180, 190, 255},
                    SDL_Color valueCol = {235, 235, 235, 255});
    void drawText_(const std::string& s, int x, int y, SDL_Color col, bool centerY = true);
    void drawCheckpoint_();
    void drawHints_();

    void renderResultsTable();
    void renderResultsBackground_();
    void renderResultsTablePanel_(const SDL_Rect& tableRect);
    void renderResultsUpgradesPanel_(const SDL_Rect& panelRect);


    SDL_Renderer* ren;
    int winW, winH;

    SDL_Texture* mapTex = nullptr;
    int mapW = 0, mapH = 0;

    SDL_Texture* carTex = nullptr;
    int atlasCols = 8, atlasRows = 2;
    int cellW = 0, cellH = 0;
    float angle0 = 0.0f;
    bool clockwise = true;

    int camX = 0, camY = 0;

    float facingDeg = 0.0f;
    int lastX = -1, lastY = -1;

    int   hudPos_       = 1;      
    int   hudPlayers_   = 2; 
    int   hudHp_        = 100;     // vida actual
    int   hudMaxHp_     = 100;    // vida máxima
    float hudSpeedKph_  = 128.f;  // velocidad simulada (km/h)

    Uint32 raceStartTicks_ = 0;
    bool   raceStarted_    = false;

    SDL_Texture* checkpointTex = nullptr;
    SDL_Texture* hintTex       = nullptr;

    int selfScreenX_ = 0;
    int selfScreenY_ = 0;
    int checkpointSizePx = 32;
    int hintSizePx       = 16;

    TTF_Font* uiFont = nullptr;
    int uiFontSize = 16;

    bool raceFinished_ = false;
    std::vector<RaceStateDTO> finalStandings_; 
    Uint32 resultsStartTicks_ = 0;

};

#endif // TP_TALLER_G7_CLIENT_DIBUJADOR_H
