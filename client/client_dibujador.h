#ifndef TP_TALLER_G7_CLIENT_DIBUJADOR_H
#define TP_TALLER_G7_CLIENT_DIBUJADOR_H

#include <SDL.h>
#include <string>

class ClientDibujador {
public:
    ClientDibujador(SDL_Renderer* r, int winW, int winH);
    ~ClientDibujador();

    bool loadMap(const std::string& pathPng);

    bool loadCarAtlas(const std::string& pathPng, int cols = 8, int rows = 2,
                      float angle0Deg = -90.0f, bool clockwise = true);

    void renderFrame(int playerX, int playerY);

    void setFacingDeg(float deg) { facingDeg = deg; }

private:
    SDL_Texture* loadTexture_(const std::string& path);

    int frameForAngle_(float angleDeg) const;

    void updateCamera_(int playerX, int playerY);

    SDL_Renderer* ren;
    int winW, winH;

    SDL_Texture* mapTex = nullptr;
    int mapW = 0, mapH = 0;

    SDL_Texture* carTex = nullptr;
    int atlasCols = 8, atlasRows = 2;
    int cellW = 0, cellH = 0;
    float angle0 = -90.0f; // frame 0 = “arriba”
    bool clockwise = true;

    int camX = 0, camY = 0;

    float facingDeg = -90.0f;
    int lastX = -1, lastY = -1;
};
#endif //TP_TALLER_G7_CLIENT_DIBUJADOR_H