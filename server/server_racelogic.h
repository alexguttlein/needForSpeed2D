#ifndef SERVER_RACELOGIC_H
#define SERVER_RACELOGIC_H

#include "../common/map.h"
#include "../common/vector2D.h"
#include "../common/constants.h"
#include "yamlLoader.h"
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <mutex>
#include <string>

class RaceLogic {

private:
    YamlLoader yamlLoader;
    Map mapData;
    std::map<int, int> nextCheckpointIndex;
    std::vector<int> finishedPlayers; // guardamos en orden los jugadores que terminaron
    std::mutex finishMutex;
    std::map<int, float> finishTimes;

    std::vector<Vector2D<float>> actualRaceCheckpoints;
    std::string actualRaceId; // ID del circuito actual
    int currentRaceId;

public:
    /*
    * Constructor de RaceLogic
    *
    * */
    RaceLogic();

    /*
    * Devuelve los checkpoints del circuito de carrera actual
    *
    * */
    std::vector<Vector2D<float>> getActualRaceCheckpoints();

    /*
    * Configura el circuito de carrera actual
    *
    * */
    void setCurrentRace();

    /*
    * Verifica si hay otro circuito disponible
    *
    * */
    bool hasNextRace();

    /*
    * Verifica si la carrera actual ha finalizado (todos los jugadores terminaron)
    *
    * */
    bool isRaceOver();

    /*
    * Configura los checkpoints del circuito actual por ID
    *
    * */
    void setCurrentCheckpoints(std::string raceId);

    /*
    * Agrega un jugador al seguimiento de checkpoints
    *
    * */
    void addPlayer(int playerId);

    /*
    * Remueve un jugador de la partida
    *
    * */
    void removePlayer(int playerId);

    /*
    * Devuelve la cantidad de jugadores activos en la carrera actual
    *
    * */
    int getActiveRacePlayers();

    /*
    * Verifica si el jugador ha cruzado el checkpoint actual
    *
    * */    
    bool checkCheckpoint(int playerId, const Vector2D<float>& currentCarPosition);

    /*
    * Verifica si ha terminado la carrera para un jugador
    *
    * */
    bool finishRace(int playerId);

    /*
    * Verifica si el jugador ha terminado la carrera
    *
    * */
    bool hasPlayerFinished(int playerId) const;

    /*
    * Devuelve la lista de jugadores que han terminado la carrera
    *
    * */
    std::vector<int> getFinishedPlayers() const;

    /*
    * Devuelve el tiempo (en segundos) en que el jugador terminó la carrera; -1 si no terminó
    *
    * */
    float getFinishTime(int playerId) const;

    /*
    * Informa a RaceLogic el tiempo actual (en segundos) para registrar cuando un jugador termina
    *
    * */
    void setCurrentRaceTimeSeconds(float currentTimeSeconds);

    /*
    * Verifica si el jugador ha completado el checkpoint actual
    *
    * */
    bool completedCheckpoint(int playerId, const Vector2D<float>& currentCarPosition, int& currentCheckpointIdx);

    /*
    * Devuelve la posición del próximo checkpoint que el jugador debe alcanzar
    * Utilizado para el minimapa/hints.
    *
    * */
    Vector2D<float> getNextCheckpointPosition(int playerId) const;

    /*
    * Devuelve una serie de hints (puntos intermedios) hacia el próximo checkpoint
    * del jugador solicitado.
    * 
    * */
    std::vector<Vector2D<float>> getHintsForPlayer(int playerId, const Vector2D<float>& currentCarPosition) const;
};
#endif // SERVER_RACELOGIC_H
