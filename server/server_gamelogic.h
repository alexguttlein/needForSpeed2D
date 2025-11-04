#ifndef SERVER_GAMELOGIC_H
#define SERVER_GAMELOGIC_H



#pragma once

#include "car.h"
#include "../common/gameSnapshot.h"
#include <map> // Para manejar los autos por ID
#include <memory>

class GameLogic {
private:
    std::map<uint8_t, std::shared_ptr<Car>> cars;
    
public:
    GameLogic();
    
    /*
    * Procesa un comando recibido para un auto específico
    *
    * */
   // void processCommand(uint8_t car_id, const std::string& command);

    /*
    * Actualiza la lógica del juego (física, estado de autos, etc.)
    *
    * */
   // void update(float dt);

    /*
    * Crea y devuelve un snapshot del estado actual del juego
    *
    * */    
    GameSnapshot getSnapshot() const;
};
#endif // SERVER_GAMELOGIC_H
