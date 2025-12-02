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
        "NEED FOR SPEED",
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

    bool running = true;
    bool havePos = false;
    bool lastRaceFinished = false;
    bool musicGameplayStarted = false;
    Snapshot snapshot;

    while (running) {
        auto start = std::chrono::steady_clock::now();

        handleSDLEvents_(running, dib, snapshot);
        processGameEvents_(running);
        updateGameState_(snapshot, havePos);
        
        if (havePos) {
            updateRaceState_(snapshot, dib, musicGameplayStarted, lastRaceFinished);
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
    dib.loadUpgradeIcons("assets/ui/escudo.png", "assets/ui/aceleracion.png",
                         "assets/ui/control.png", "assets/ui/velocidad-maxima.png");
}

void Client::initAudio_() {
    try {
        audioManager_ = std::make_unique<AudioManager>();
        
        audioManager_->loadMusic(AudioManager::MUSIC_LOBBY, "assets/sound/musica-menu.mp3");
        audioManager_->loadMusic(AudioManager::MUSIC_GAMEPLAY, "assets/sound/musica-carrera.mp3");
        
        audioManager_->loadSound(AudioManager::SFX_ACCELERATION, "assets/sound/aceleracion.mp3");
        audioManager_->loadSound(AudioManager::SFX_BRAKE, "assets/sound/freno.mp3");
        audioManager_->loadSound(AudioManager::SFX_COLLISION_CAR, "assets/sound/colision-autos.mp3");
        audioManager_->loadSound(AudioManager::SFX_COLLISION_BUILDING, "assets/sound/colision-edificios.mp3");
        
        audioManager_->setMusicVolume(1);
        audioManager_->setSoundVolume(20);
        
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

void Client::handleMovementKey_(SDL_Keycode key, bool pressed, ClientDibujador& dib) {
    if (dib.hasPlayerFinishedRace()) return;
    
    commandMessage cmd;
    cmd.key = static_cast<SDL_KeyCode>(key);
    cmd.isPressed = pressed;
    commandQueue.push(cmd);
    
    // Manejar sonido de aceleración para W y S
    if ((key == SDLK_w || key == SDLK_s) && audioManager_) {
        if (pressed && accelerationChannel_ == -1) {
            accelerationChannel_ = audioManager_->playSound(AudioManager::SFX_ACCELERATION, -1);
        } else if (!pressed && accelerationChannel_ != -1) {
            audioManager_->stopSound(accelerationChannel_);
            accelerationChannel_ = -1;
        }
    }
}

void Client::handleUpgradeKey_(int upgradeId, ClientDibujador& dib) {
    commandMessage cmd;
    cmd.key = static_cast<SDL_KeyCode>(SDLK_0 + upgradeId);
    cmd.isPressed = true;
    commandQueue.push(cmd);
    dib.showUpgradePopup(upgradeId);
}

void Client::handleKeyDown_(SDL_Keycode key, ClientDibujador& dib, bool raceFinished, bool playerFinished) {
    switch (key) {
        case SDLK_w:
        case SDLK_s:
        case SDLK_a:
        case SDLK_d:
            // Solo permitir movimiento durante la carrera (no terminada) y el jugador no ha terminado
            if (!raceFinished && !playerFinished) {
                handleMovementKey_(key, true, dib);
            }
            break;
        case SDLK_1: 
            // Solo permitir mejoras en fase de compras (carrera terminada, juego no terminado)
            if (raceFinished) handleUpgradeKey_(1, dib); 
            break;
        case SDLK_2: 
            if (raceFinished) handleUpgradeKey_(2, dib); 
            break;
        case SDLK_3: 
            if (raceFinished) handleUpgradeKey_(3, dib); 
            break;
        case SDLK_4: 
            if (raceFinished) handleUpgradeKey_(4, dib); 
            break;
        default: break;
    }
}

void Client::handleKeyUp_(SDL_Keycode key, ClientDibujador& dib, bool raceFinished, bool playerFinished) {
    // Solo procesar key up durante la carrera
    if (!raceFinished && !playerFinished) {
        switch (key) {
            case SDLK_w:
            case SDLK_s:
            case SDLK_a:
            case SDLK_d:
                handleMovementKey_(key, false, dib);
                break;
            default: break;
        }
    }
}

void Client::handleSDLEvents_(bool& running, ClientDibujador& dib, const Snapshot& snapshot) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || 
            (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q))) {
            running = false;
        } else if (e.type == SDL_KEYDOWN) {
            handleKeyDown_(e.key.keysym.sym, dib, snapshot.raceFinished, hasFinished_);
        } else if (e.type == SDL_KEYUP) {
            handleKeyUp_(e.key.keysym.sym, dib, snapshot.raceFinished, hasFinished_);
        }
    }
}

void Client::processGameEvents_(bool& running) {
    Event evt;
    while (eventQueue.try_pop(evt)) {
        if (evt.type == EventType::SERVER_DISCONNECTED) {
            std::cout << "debug: Servidor desconectado, cerrando cliente..." << std::endl;
            running = false;
        }
    }
}

void Client::updateGameState_(Snapshot& snapshot, bool& havePos) {
    Snapshot snapTmp;
    while (snapshotQueue.try_pop(snapTmp)) {
        snapshot = std::move(snapTmp);
        havePos = true;
    }
}

void Client::handleCollisionSounds_(const Snapshot& snapshot, int myId) {
    if (!audioManager_) return;
    
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

void Client::handleBrakeSound_(const Snapshot& snapshot, int myId) {
    if (!audioManager_) return;
    
    bool playerIsBraking = false;
    for (const auto& car : snapshot.cars) {
        if (car.car_id == myId && car.isBraking) {
            playerIsBraking = true;
            break;
        }
    }
    
    if (playerIsBraking && brakeChannel_ == -1) {
        brakeChannel_ = audioManager_->playSound(AudioManager::SFX_BRAKE, 0);
    } else if (!playerIsBraking && brakeChannel_ != -1) {
        audioManager_->stopSound(brakeChannel_);
        brakeChannel_ = -1;
    }
}

void Client::handleAudioEffects_(const Snapshot& snapshot, int myId) {
    // Verificar si el jugador ha terminado
    bool justFinished = false;
    for (const auto& rs : snapshot.raceStates) {
        if (rs.playerId == myId && rs.hasFinished) {
            if (!hasFinished_) {
                justFinished = true;
                hasFinished_ = true;
            }
            break;
        }
    }
    
    // Si acaba de terminar, detener todos los sonidos de movimiento
    if (justFinished && audioManager_) {
        if (accelerationChannel_ != -1) {
            audioManager_->stopSound(accelerationChannel_);
            accelerationChannel_ = -1;
        }
        if (brakeChannel_ != -1) {
            audioManager_->stopSound(brakeChannel_);
            brakeChannel_ = -1;
        }
    }
    
    // Solo reproducir sonidos si no ha terminado
    if (!hasFinished_) {
        handleCollisionSounds_(snapshot, myId);
        handleBrakeSound_(snapshot, myId);
    }
}

void Client::updateRaceState_(const Snapshot& snapshot, ClientDibujador& dib, 
                               bool& musicGameplayStarted, bool& lastRaceFinished) {
    // Encontrar el estado de carrera del jugador
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
    
    // Iniciar música de gameplay y resetear flag de terminado
    if (!musicGameplayStarted && !snapshot.raceFinished && audioManager_) {
        audioManager_->playMusic(AudioManager::MUSIC_GAMEPLAY, 1500);
        musicGameplayStarted = true;
        hasFinished_ = false;  // Resetear al inicio de nueva carrera
    }

    // Juego terminado
    if (snapshot.gameFinished) {
        dib.setGameFinished(true, snapshot.leaderboards);
        if (audioManager_) {
            audioManager_->stopMusic(1000);
        }
    }

    // Carrera terminada
    if (snapshot.raceFinished && !lastRaceFinished) {
        dib.setRaceFinished(true, snapshot.raceStates);
    }
    if (!snapshot.raceFinished && lastRaceFinished) {
        dib.setRaceFinished(false, {});
        if (dib.isUpgradePopupVisible()) {
            dib.hideUpgradePopup();
        }
        // Resetear flag cuando comienza nueva carrera
        hasFinished_ = false;
    }
    lastRaceFinished = snapshot.raceFinished;
    
    // Efectos de audio
    handleAudioEffects_(snapshot, myId);
    
    // Renderizar
    if (myRace) {
        dib.renderAll(snapshot.cars, myId, myRace->timeLeftRace, snapshot.raceStates);
    }
}
