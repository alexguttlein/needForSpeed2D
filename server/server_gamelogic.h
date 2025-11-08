#ifndef SERVER_GAMELOGIC_H
#define SERVER_GAMELOGIC_H

#pragma once

#include "car.h"
#include "../common/snapshot.h"
#include "../common/eventType.h"
#include "server_raceBuilder.h"
#include "box2d/box2d.h"
#include <map> 
#include <memory>

class GameLogic {
private:
    RaceBuilder raceBuilder;
    b2WorldId world;
    std::map<int, std::shared_ptr<Car>> cars;
    int lastCommandPlayerId = 0;
    
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
   void processCommand(int car_id, const std::string& command, bool isPressed);

    /*
    * Actualiza la lógica del juego (física, estado de autos, etc.)
    * Por ahora solo actualiza la posición de los autos.
    * */
   void update();

    /*
    * Crea y devuelve un snapshot del estado actual del juego
    *
    * */    
    std::shared_ptr<Snapshot> getSnapshot(EventType controlEvent)const;

    /*
    * Agrega un auto al juego para un jugador específico
    *
    * */
    void addCar(int playerId, int carType);
};
#endif // SERVER_GAMELOGIC_H
