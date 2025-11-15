#ifndef SERVER_GAMELOGIC_H
#define SERVER_GAMELOGIC_H

#pragma once

#include "car.h"
#include "../common/snapshot.h"
#include "../common/eventType.h"

//#include "mapLoader.h"
#include "server_raceBuilder.h"
#include "server_racelogic.h"
#include "box2d/box2d.h"
//#include "mapSetObjects.h"


#include <map> 
#include <memory>
#include <iostream>
#include <cmath>

class GameLogic {
private:
    
    //MapLoader mapLoader;
    RaceBuilder raceBuilder;
    //MapSetObjects mapSetObjects;
    RaceLogic raceLogic;
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
    * 
    * */
   void update(int currentTick);

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

    /*
    * Verifica y maneja las colisiones entre autos
    *
    * */
    void checkCollisions();

    /*
    * Aplica daño a los autos involucrados en una colisión
    *
    * */
    void applyCollisionDamage(Car* carA, Car* carB, b2Vec2 normal, float hitSpeed);

    /*
    * Obtiene la velocidad de aproximación entre dos cuerpos en colisión
    *
    * */
    float getCollisionSpeed(b2BodyId bodyA, b2BodyId bodyB);

    /*
    * Obtiene la normal de colisión entre dos cuerpos
    *
    * */
    b2Vec2 getCollisionNormal(b2BodyId bodyA, b2BodyId bodyB);
};
#endif // SERVER_GAMELOGIC_H
