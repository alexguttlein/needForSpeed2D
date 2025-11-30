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
#include "npcData.h"


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
    int remainingFinishTicks = Constants::MAX_TICKS;
    int lastTickChecked = 0;


    RaceState raceState = IN_PROGRESS;

    YamlLoader mapLoader;
    Leaderboard leaderboard;
    RaceBuilder raceBuilder;
    MapSetObjects mapSetObjects;
    RaceLogic raceLogic;
    b2WorldId world;

    std::map<int, std::shared_ptr<Car>> cars;
    std::map<int ,std::shared_ptr<Car>> staticNpcs; // NPCs estáticos en el mapa
    std::vector<NPCData> npcsData;
    
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
    void addCar(int playerId, int carType, const std::string& playerName);

    /*
    * Carga los NPCs estáticos definidos en el mapa
    *
    * */
    void loadStaticNpcs();

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
    * Resetea la posición de los NPCs estáticos para la siguiente carrera
    *
    * */
    void resetNpcs();

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
    * Verifica si se debe finalizar una carrera por tiempo y actualiza el estado
    * si en 10 minutos no se han terminado una carrera finaliza la carrera
    * */
    void checkFinishRaceByTime(int currentTick);

    /*
    * Verifica si la carrera ha terminado
    *
    * */
    void hasRaceOver(int currentTick);

    /*
    * setea que el jugador no terminó la carrera, y la termina automáticamente
    * como no termino, se le aplica la penalización correspondiente.
    * */
    void setFinishRaceByPlayerLeftTimeToFinish(int playerId, int currentRaceTime);

    /*
    * Resetea el temporizador para la siguiente carrera
    *
    */
    void resetRaceTemporizer();

    /*
    * Obtiene el temporizador de la carrera para mostrar a los jugadores
    * Si eligio una mejora se le resta el tiempo correspondiente
    * */
    std::string getRaceTimerForPlayer(int playerId);

    /*
    * Convierte ticks a formato "MM:SS"
    *
    * */
    std::string getMinuteSecondFromTicks(int ticks, bool finished);

    /*
    * Obtiene los segundos totales restantes considerando las mejoras aplicadas
    *
    * */
    int getRemainingSecondsForPlayer(int playerId);


    /*
    * Obtiene el tiempo de penalización asociado a una mejora específica
    *
    * */
    float getPenalizedTimeUpgrade(int upgradeId) const;

    /*
    * Destructor de GameLogic.
    *
    * */
    ~GameLogic();
};
#endif // SERVER_GAMELOGIC_H
