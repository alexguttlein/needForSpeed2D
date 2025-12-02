#include "client_class.h"
#include "client_dibujador.h"
#include "client_qtManager.h"

#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>
#include <chrono>
#include <cstdlib>

using ms = std::chrono::milliseconds;
constexpr int FPS = 60;
const ms FRAME_MS {1000 / FPS };

Client::Client(const char* host, const char* port) :
    protocol(host, port), snapshotQueue(100), commandQueue(100), eventQueue(10),
    receiver(protocol, snapshotQueue, eventQueue), sender(protocol, commandQueue), playing(false) {}

void Client::run() {
    receiver.start();
    sender.start();

    initAudio_();
    
    if (audioManager_) {
        audioManager_->playMusic(AudioManager::MUSIC_LOBBY, 1000);
    }

    // menu inicial Qt
    ClientQtManager qt(this);
    qt.start();

    // si no se inicio una partida, no abre SDL
    if (!playing) return;

    if (audioManager_) {
        audioManager_->stopMusic(500);
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return;
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
        std::fprintf(stderr, "IMG_Init PNG error: %s\n", IMG_GetError());
        SDL_Quit(); return;
    }

    const int W = 640, H = 480;
    SDL_Window* win = SDL_CreateWindow(
        "Cliente - Mapa + Auto",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 0);
    if (!win) {
        std::fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        std::fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    ClientDibujador dib(ren, W, H);

    loadTexturesAndAssets_(dib);
    dib.setFacingDeg(0.0f);
    
    // Configurar el sistema de cheats
    dib.setCheatCallback([this](const std::string& cheatCode) {
        // Enviar cheat al servidor
        protocol.sendCheat(cheatCode);
        std::cout << "[Client] Cheat activado: " << cheatCode << std::endl;
    });

    bool running = true;
    bool havePos = false;

    Snapshot snapshot;
    bool lastRaceFinished = false;
    bool musicGameplayStarted = false; 

    while (running) {
        auto start = std::chrono::steady_clock::now();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                
                dib.processKeyForCheat(e.key.keysym.sym); // Procesar tecla para detección de cheats
                
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: running = false; break;
                    case SDLK_q: running = false; break;
                    case SDLK_w: 
                        if (!dib.hasPlayerFinishedRace()) {
                            commandQueue.push({ SDLK_w, true });
                            if (audioManager_ && accelerationChannel_ == -1) {
                                accelerationChannel_ = audioManager_->playSound(
                                    AudioManager::SFX_ACCELERATION, -1); // -1 = loop infinito
                            }
                        }
                        break;
                    case SDLK_s: 
                        if (!dib.hasPlayerFinishedRace()) {
                            commandQueue.push({ SDLK_s, true });
                            if (audioManager_ && accelerationChannel_ == -1) {
                                accelerationChannel_ = audioManager_->playSound(
                                    AudioManager::SFX_ACCELERATION, -1); // -1 = loop infinito
                            }
                        }
                        break;
                    case SDLK_a:
                        if (!dib.hasPlayerFinishedRace())
                            commandQueue.push({ SDLK_a, true });
                        break;
                    case SDLK_d:
                        if (!dib.hasPlayerFinishedRace())
                            commandQueue.push({ SDLK_d, true });
                        break;
                    case SDLK_1:
                        commandQueue.push({ SDLK_1, true });
                        dib.showUpgradePopup(1);
                        break;
                    case SDLK_2:
                        commandQueue.push({ SDLK_2, true });
                        dib.showUpgradePopup(2);
                        break;
                    case SDLK_3:
                        commandQueue.push({ SDLK_3, true });
                        dib.showUpgradePopup(3);
                        break;
                    case SDLK_4:
                        commandQueue.push({ SDLK_4, true });
                        dib.showUpgradePopup(4);
                        break;
                }
            }
            else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_w: 
                        if (!dib.hasPlayerFinishedRace()) {
                            commandQueue.push({ SDLK_w, false });
                            if (audioManager_ && accelerationChannel_ != -1) {
                                audioManager_->stopSound(accelerationChannel_);
                                accelerationChannel_ = -1;
                            }
                        }
                        break;
                    case SDLK_s: 
                        if (!dib.hasPlayerFinishedRace()) {
                            commandQueue.push({ SDLK_s, false });
                            if (audioManager_ && accelerationChannel_ != -1) {
                                audioManager_->stopSound(accelerationChannel_);
                                accelerationChannel_ = -1;
                            }
                        }
                        break;
                    case SDLK_a:
                        if (!dib.hasPlayerFinishedRace())
                            commandQueue.push({ SDLK_a, false });
                        break;
                    case SDLK_d:
                        if (!dib.hasPlayerFinishedRace())
                            commandQueue.push({ SDLK_d, false });
                        break;
                }
            }
        }

        Snapshot snapTmp;
        while (snapshotQueue.try_pop(snapTmp)) {
            snapshot = std::move(snapTmp); // Siempre guardamos el más reciente
            havePos = true;
        }

        //se revisa la queue de eventos para ver si el server se desconecto
        Event evt;
        while (eventQueue.try_pop(evt)) {
            if (evt.type == EventType::SERVER_DISCONNECTED) {
                std::cout << "debug: Servidor desconectado, cerrando cliente..." << std::endl;
                running = false;
            }
        }

        if (havePos) {
            int myId = selfId.load();
            const RaceStateDTO* myRace = nullptr;
            for (const auto& rs : snapshot.raceStates) {
                if (rs.playerId == myId) {
                    myRace = &rs;
                    break;
                }
            }

            if (myRace) {
                dib.updateRaceState(*myRace);
            }
            
            if (!musicGameplayStarted && !snapshot.raceFinished && audioManager_) {
                audioManager_->playMusic(AudioManager::MUSIC_GAMEPLAY, 1500);
                musicGameplayStarted = true;
            }

            if (snapshot.gameFinished) {
                dib.setGameFinished(true, snapshot.leaderboards);
                if (audioManager_) {
                    audioManager_->stopMusic(1000);
                }
            }

            if (snapshot.raceFinished && !lastRaceFinished) {
                dib.setRaceFinished(true, snapshot.raceStates);
            }
            if (!snapshot.raceFinished && lastRaceFinished) {
                dib.setRaceFinished(false, {});
                if (dib.isUpgradePopupVisible()) {
                    dib.hideUpgradePopup();
                }
            }
            lastRaceFinished = snapshot.raceFinished;
            
            if (audioManager_) {
                for (const auto& collision : snapshot.collisions) {
                    if (collision.playerId == myId) {
                        if (collision.collisionType == EventType::COLLISION_CAR) {
                            audioManager_->playSound(AudioManager::SFX_COLLISION_CAR);
                        } else if (collision.collisionType == EventType::COLLISION_BUILDING) {
                            audioManager_->playSound(AudioManager::SFX_COLLISION_BUILDING);
                        }
                    }
                }
            }
            dib.renderAll(snapshot.cars, selfId.load(), myRace->timeLeftRace);
        }
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<ms>(end - start);
        if (elapsed < FRAME_MS) {
            std::this_thread::sleep_for(FRAME_MS - elapsed);
        }
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
}

Queue<Event>& Client::getEventQueue() {
    return eventQueue;
}

ClientProtocol& Client::getProtocol() {
    return protocol;
}

Queue<Snapshot> & Client::getSnapshotQueue() {
    return snapshotQueue;
}

void Client::setSelfId(int id) {
    selfId = id;
}

int Client::getSelfId() const {
    return selfId.load();
}

bool Client::sendLobbyOption(const std::string& option, const std::string& name, const int& carId) {
    return protocol.sendLobbyOption(option, name, carId);
}

void Client::loadTexturesAndAssets_(ClientDibujador& dib) {
    if (!dib.loadMap("assets/need-for-speed/cities/Liberty.png", "assets/need-for-speed/cities/Liberty_BRIDGES.png")) {
        std::fprintf(stderr, "No pude cargar assets/maps/iberty.png\n");
    }
    for(int i = 1; i <=7; ++i) {
        std::string path = "assets/need-for-speed/cars/auto-" + std::to_string(i) + ".png";
        if (!dib.loadCarAtlasForId(i, path, 8, 2, 0.0f, true)) {
            std::fprintf(stderr, "No pude cargar auto %d desde %s\n", i, path.c_str());
        }
    }
    dib.setUIFont("assets/ui/FreeSans.ttf", 16);
    dib.loadCheckpoint("assets/ui/checkpoint.png");
    dib.loadHint("assets/ui/hint.png");
}

void Client::initAudio_() {
    try {
        audioManager_ = std::make_unique<AudioManager>();
        
        audioManager_->loadMusic(AudioManager::MUSIC_LOBBY, "assets/sound/musica-menu.mp3");
        audioManager_->loadMusic(AudioManager::MUSIC_GAMEPLAY, "assets/sound/musica-carrera.mp3");
        
        audioManager_->loadSound(AudioManager::SFX_ACCELERATION, "assets/sound/aceleracion.mp3");
        audioManager_->loadSound(AudioManager::SFX_COLLISION_CAR, "assets/sound/colision-autos.mp3");
        audioManager_->loadSound(AudioManager::SFX_COLLISION_BUILDING, "assets/sound/colision-edificios.mp3");
        
        audioManager_->setMusicVolume(50);
        audioManager_->setSoundVolume(100);
        
        std::cout << "[Client] Sistema de audio inicializado correctamente" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[Client] Error al inicializar audio: " << e.what() << std::endl;
        std::cerr << "[Client] El juego continuará sin audio" << std::endl;
    }
}

void Client::changePlayingStatus() {
    playing = !playing;
}

void Client::setGameId(int id) {
    gameId = id;
}

int Client::getGameId() {
    return gameId;
}
