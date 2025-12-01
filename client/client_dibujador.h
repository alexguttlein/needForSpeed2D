#ifndef TP_TALLER_G7_CLIENT_DIBUJADOR_H
#define TP_TALLER_G7_CLIENT_DIBUJADOR_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "../common/carStateDTO.h"
#include "../common/constants.h"
#include "../common/raceStateDTO.h"
#include "../server/leaderBoard.h"
#include <unordered_map>

struct CarAtlas {
    SDL_Texture* tex = nullptr;
    int cols = 0, rows = 0;
    int cellW = 0, cellH = 0;
    float angle0 = 0.f;
    bool clockwise = true;
};

class ClientDibujador {
public:
    ClientDibujador(SDL_Renderer* r, int winW, int winH);
    ~ClientDibujador();

    bool loadMap(const std::string& pathPng, const std::string& pathPngOver);
    bool setUIFont(const std::string& ttfPath, int size = 16);

    void setFacingDeg(float deg) { facingDeg = deg; }

    void renderAll(const std::vector<CarStateDTO>& cars, int selfId, std::string timeLeftRace);
    void updateRaceState(const RaceStateDTO& raceState);
    bool loadCarAtlasForId(int carTypeId, const std::string& pathPng,
                           int cols, int rows, float angle0Deg, bool cw);

    void setHUDSpeedKph(float kph)          { hudSpeedKph_ = kph; }
    void setHUDHp(int hp, int maxHp)        { hudHp_ = hp; hudMaxHp_ = maxHp; }

    bool loadCheckpoint(const std::string& pathPng);
    bool loadHint(const std::string& pathPng);

    void setRaceFinished(bool finished, const std::vector<RaceStateDTO>& standings);
    
    void showUpgradePopup(int upgradeId);
    void hideUpgradePopup();
    bool isUpgradePopupVisible() const { return showUpgradePopup_; }
    
    void setGameFinished(bool finished, const std::vector<PlayerTime>& leaderboard);
    bool isGameFinished() const { return gameFinished_; }
    
    bool hasPlayerFinishedRace() const { return playerFinishedRace_; }

    Vector2D<float> hudNextCheckpoint_{};               
    std::vector<Vector2D<float>> hudHints_{};

private:
    SDL_Texture* loadTexture_(const std::string& path);
    void updateCamera_(int playerX, int playerY);

    void drawHUD_(std::string timeLeftRace);
    void drawHudSpeed_(int panelX, int panelY);
    void drawHudHealth_(int panelX, int panelY);
    void drawHudRace_(int panelX, int panelY);
    void drawHudTime_(int panelX, int panelY, int panelW, std::string timeLeftRace);
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
    
    void renderUpgradePopup_();
    std::string getUpgradeName_(int upgradeId) const;
    std::string getUpgradeDescription_(int upgradeId) const;
    
    void renderGameOver_();
    void renderWaitingForPlayers_();

    const CarAtlas* atlasFor(int carTypeId) const;

    int frameForAngle_(float angleDeg, const CarAtlas& atlas) const;

    SDL_Renderer* ren;
    int winW, winH;

    SDL_Texture* mapTex = nullptr;
    SDL_Texture* mapOverTex = nullptr;
    int mapW = 0, mapH = 0;

    SDL_Texture* carTex = nullptr;
    int atlasCols = 8, atlasRows = 2;
    int cellW = 0, cellH = 0;
    float angle0 = 0.0f;
    bool clockwise = true;

    int camX = 0, camY = 0;

    float facingDeg = 0.0f;
    int lastX = -1, lastY = -1;

    int   currentCheckpoint = 0;      
    int   numberOfCheckpoints = Constants::NUMBER_OF_CHECKPOINTS;
    int   hudHp_        = 100;
    int   hudMaxHp_     = 100;
    float hudSpeedKph_  = 0.0f;

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
    
    bool showUpgradePopup_ = false;
    int selectedUpgradeId_ = 0;
    Uint32 upgradePopupStartTicks_ = 0;
    
    bool gameFinished_ = false;
    std::vector<PlayerTime> finalLeaderboard_;
    
    bool playerFinishedRace_ = false;

    std::unordered_map<int, CarAtlas> carAtlases_;
};

#endif // TP_TALLER_G7_CLIENT_DIBUJADOR_H
