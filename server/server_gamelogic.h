#ifndef SERVER_GAMELOGIC_H
#define SERVER_GAMELOGIC_H

#pragma once

#include "car.h"
#include "../common/snapshot.h"
#include "../common/eventType.h"
#include "../common/constants.h"

#include "yamlLoader.h"
#include "leaderBoard.h"
#include "server_raceBuilder.h"
#include "server_racelogic.h"
#include "box2d/box2d.h"
#include "mapSetObjects.h"


#include <map> 
#include <memory>
#include <iostream>
#include <cmath>


 enum RaceState {
    IN_PROGRESS,            // La carrera está en progreso
    WAITING_FOR_TRANSITION, // La carrera ha terminado, estamos en el periodo de espera (10s)
    GAME_OVER               // No hay más circuitos
};


class GameLogic {
private:
    
    int raceStartTick = 0;
    int transitionStartTick = 0;
    RaceState raceState = IN_PROGRESS;

    YamlLoader mapLoader;
    Leaderboard leaderboard;
    RaceBuilder raceBuilder;
    MapSetObjects mapSetObjects;
    RaceLogic raceLogic;
    b2WorldId world;
    std::map<int, std::shared_ptr<Car>> cars;
    int lastCommandPlayerId = 0;

    std::map<int, bool> hasSelectedUpgrade;
    std::map<int, int> selectedUpgradeId;

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
    * Procesa la selección de mejora de un jugador durante el estado WAITING_FOR_TRANSITION
    *
    * */
   void processUpgradeSelection(int car_id, const std::string& command);

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

    /*
    * Simula la lógica de la carrera cuando está en transición
    *
    * */
    void simulateRaceInTransition(int currentTick);

    /*
    * Aplica la mejora seleccionada a un auto
    *
    */
    void applyUpgradeToCar();

    /*
    * Resetea el estado de la carrera para iniciar una nueva ronda
    *
    * */
    void resetFinishRace();

    /*
    * Simula la lógica de la carrera cuando está en progreso
    *
    * */
    void simulateRaceInProgress(int currentTick, float currentRaceTime);

    /*
    * Simula la física de la carrera por un intervalo de tiempo dt
    *
    * */
    void simulateRacePhysics(const float dt, int currentTick);

    /*
    * Convierte el tick actual a tiempo en segundos
    *
    * */
    float getCurrentTimeSeconds(int currentTick);

    /*
    * si hay otra carrera disponible, setea el estado a WAITING_FOR_TRANSITION
    *
    * */
    void setTransition(int currentTick);

    /*
    * Finaliza el juego, actualizando el estado correspondiente
    *
    * */
    void finishGame();

    /*
    * Verifica si se debe finalizar el juego por tiempo y actualiza el estado
    * si en 10 minutos no se han terminado una carrera finaliza el game.
    * */
    void checkFinishGameByTime(int currentTick);

    ~GameLogic();
};
#endif // SERVER_GAMELOGIC_H
