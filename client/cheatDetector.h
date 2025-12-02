#ifndef CHEAT_DETECTOR_H
#define CHEAT_DETECTOR_H

#include <vector>
#include <chrono>
#include <string>
#include <functional>
#include <unordered_map>
#include <SDL.h>

/*
* Estructura para definir una secuencia de cheats
*
* */
struct CheatSequence {
    std::vector<SDL_KeyCode> keys;
    std::string cheatCode;
    std::chrono::milliseconds timeWindow;
    
    CheatSequence(std::vector<SDL_KeyCode> keySeq, std::string code, int timeWindowMs = 3000)
        : keys(std::move(keySeq)), cheatCode(std::move(code)), timeWindow(timeWindowMs) {}
};

class CheatDetector {
    
public:
    
    /*
    * Tipo de callback para notificar cuando se detecta un cheat
    *
    * */
    using CheatCallback = std::function<void(const std::string&)>;
    
    /*
    * Constructor de CheatDetector
    *
    * */
    CheatDetector();

    /*
    * Destructor de CheatDetector
    *
    * */
    ~CheatDetector() = default;
    
    /*
    * Configura los cheats disponibles
    *
    * */
    void setupCheats();
    
    /*
    * Procesa una tecla presionada para detectar cheats
    *
    * */
    void processKeyPress(SDL_KeyCode key);
    
    /*
    * Establece el callback que se llama cuando se detecta un cheat
    *
    * */
    void setCheatCallback(CheatCallback callback);
    
    /*
    * Limpia el historial de teclas presionadas
    *
    * */
    void clearHistory();
    
private:

    /*
    * Estructura para almacenar una tecla presionada junto con su timestamp
    *
    * */
    struct KeyPress {
        SDL_KeyCode key;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    std::vector<CheatSequence> cheats;
    std::vector<KeyPress> keyHistory;
    CheatCallback onCheatDetected;
    
    /*
    * verifica si una secuencia coincide con algún cheat
    *
    * */
    bool checkCheatSequences();
    
    /*
    * Limpia teclas antiguas del historial
    *
    * */
    void cleanOldKeys();
    
    /*
    * Obtiene las teclas recientes dentro de la ventana de tiempo
    *
    * */
    std::vector<SDL_KeyCode> getRecentKeys(std::chrono::milliseconds timeWindow) const;
};
#endif // CHEAT_DETECTOR_H
