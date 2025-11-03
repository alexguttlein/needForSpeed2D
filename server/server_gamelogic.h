#ifndef SERVER_GAMELOGIC_H
#define SERVER_GAMELOGIC_H



#pragma once

#include "car.h"
#include "../common/gameSnapshot.h"
#include <map> // Para manejar los autos por ID
#include <memory>

class GameLogic {
private:
    std::map<int, std::shared_ptr<Car>> cars;
    
public:

    /*
    * Constructor de GameLogic. (por ahora vacío)
    *
    * */
    GameLogic();
    
    /*
    * Procesa un comando recibido para un auto específico
    *
    * */
   void processCommand(int car_id, const std::string& command);

    /*
    * Actualiza la lógica del juego (física, estado de autos, etc.)
    * Por ahora solo actualiza la posición de los autos.
    * */
    void update(float dt);

    /*
    * Crea y devuelve un snapshot del estado actual del juego
    *
    * */    
    GameSnapshot getSnapshot() const;
};
#endif // SERVER_GAMELOGIC_H
