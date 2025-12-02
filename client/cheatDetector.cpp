#include "cheatDetector.h"
#include <algorithm>
#include <iostream>

CheatDetector::CheatDetector() {
    setupCheats();
}

void CheatDetector::setupCheats() {
    cheats.clear();
    
    // Cheat para vida infinita: I-D-D-Q-D (como en Doom)
    cheats.emplace_back(
        std::vector<SDL_KeyCode>{SDLK_j, SDLK_e, SDLK_z, SDLK_u, SDLK_z},
        "GOD_MODE", 
        3000
    );
    
    // Cheat para ganar automáticamente: W-I-N-N-E-R
    cheats.emplace_back(
        std::vector<SDL_KeyCode>{SDLK_w, SDLK_i, SDLK_n, SDLK_n, SDLK_e, SDLK_r},
        "INSTANT_WIN",
        4000
    );
    
    // Cheat para matar al jugador: K-I-L-L
    cheats.emplace_back(
        std::vector<SDL_KeyCode>{SDLK_k, SDLK_i, SDLK_l, SDLK_l},
        "KILL",
        2500
    );
    
    // Cheat para velocidad máxima: T-U-R-B-O
    cheats.emplace_back(
        std::vector<SDL_KeyCode>{SDLK_t, SDLK_u, SDLK_r, SDLK_b, SDLK_o},
        "TURBO_MODE",
        3000
    );
    
    // Cheat para vida al máximo: H-E-A-L-T-H
    cheats.emplace_back(
        std::vector<SDL_KeyCode>{SDLK_h, SDLK_e, SDLK_a, SDLK_l, SDLK_t, SDLK_h},
        "MAX_HEALTH",
        4000
    );
    
    // Cheat para resetear efectos: R-E-S-E-T
    cheats.emplace_back(
        std::vector<SDL_KeyCode>{SDLK_r, SDLK_e, SDLK_s, SDLK_e, SDLK_t},
        "RESET_CHEATS",
        3000
    );
}

void CheatDetector::processKeyPress(SDL_KeyCode key) {
    auto now = std::chrono::steady_clock::now();
    keyHistory.push_back({key, now});

    cleanOldKeys();
    if (checkCheatSequences()) {
        clearHistory();
    }
}


void CheatDetector::setCheatCallback(CheatCallback callback) {
    onCheatDetected = std::move(callback);
}


void CheatDetector::clearHistory() {
    keyHistory.clear();
}


bool CheatDetector::checkCheatSequences() {
    for (const auto& cheat : cheats) {
        auto recentKeys = getRecentKeys(cheat.timeWindow);
        
        if (recentKeys.size() >= cheat.keys.size()) {
            for (size_t i = 0; i <= recentKeys.size() - cheat.keys.size(); ++i) {
                bool matches = true;
                for (size_t j = 0; j < cheat.keys.size(); ++j) {
                    if (recentKeys[i + j] != cheat.keys[j]) {
                        matches = false;
                        break;
                    }
                }
                if (matches) {
                    std::cout << "[CheatDetector] Cheat activado: " << cheat.cheatCode << std::endl;
                    if (onCheatDetected) {
                        onCheatDetected(cheat.cheatCode);
                    }
                    return true;
                }
            }
        }
    }
    return false;
}


void CheatDetector::cleanOldKeys() {
    auto now = std::chrono::steady_clock::now();
    auto maxTimeWindow = std::chrono::milliseconds(5000); // Mantener máximo 5 segundos
    
    keyHistory.erase(
        std::remove_if(keyHistory.begin(), keyHistory.end(),
            [now, maxTimeWindow](const KeyPress& kp) {
                return (now - kp.timestamp) > maxTimeWindow;
            }),
        keyHistory.end()
    );
}


std::vector<SDL_KeyCode> CheatDetector::getRecentKeys(std::chrono::milliseconds timeWindow) const {
    auto now = std::chrono::steady_clock::now();
    std::vector<SDL_KeyCode> recentKeys;
    
    for (const auto& keyPress : keyHistory) {
        if ((now - keyPress.timestamp) <= timeWindow) {
            recentKeys.push_back(keyPress.key);
        }
    }
    
    return recentKeys;
}