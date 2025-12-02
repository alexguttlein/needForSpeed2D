#ifndef SERVER_RACELOGIC_H
#define SERVER_RACELOGIC_H

#include "../common/map.h"
#include "../common/vector2D.h"
#include "../common/constants.h"
#include "yamlLoader.h"
#include "leaderBoard.h"
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
    std::unordered_map<int, RaceSpawnData> spawnData; //posicion inicial de autos en carreras
    std::map<int, int> nextCheckpointIndex;
    std::vector<int> finishedPlayers; // guardamos en orden los jugadores que terminaron
    std::mutex finishMutex;
    std::map<int, float> finishTimes;
    std::map<int, float> allTimeFinishTimes;
    std::map<int, std::string> playerNames;
    std::map<int, int> playerTimePenalties; // Penalizaciones de tiempo en ticks para cada jugador

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
    * obtiene el size de los checkpoints del circuito actual
    *
    * */ 
    int getCheckpointsSize() const;

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
    * Agrega un jugador con su nombre 
    *
    * */
    void addPlayerWithName(int playerId, const std::string& playerName);

    /*
    * Devuelve el nombre del jugador por su ID
    *
    * */
    std::string getPlayerName(int playerId) const;

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
    * Agrega un jugador a la lista de finalizados
    *
    * */
    void addFinishedPlayer(int playerId);

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
    * agrega el tiempo que hizo en carrera al tiempo total del jugador
    *
    * */
    void addTimeFinishPlayer(float addedTime, int playerId);

    /*
    * penaliza el tiempo total del jugador por haber comprado una mejora
    *
    * */
    void upgradePenalizeTimeToPlayer(float penalizeTime, int playerId);

    /*
    * Devuelve el tiempo total de finalización del jugador
    *
    * */
    float getAllTimeFinishTime(int playerId);

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

    /*
    * Devuelve el ID del circuito de carrera actual - numero de carrera 
    *
    * */
    int getCurrentRaceId() const;

    /*
    * Genera el leaderboard final basado en los tiempos de finalización de los jugadores
    * devuelve un vector ordenado por tiempo ascendente
    * 
    * */
    Leaderboard getLeaderBoard() const;

    /*
    * Devuelve la posición de spawn para un jugador según su índice
    *
    * */
    Vector2D<float> getSpawnPositionForPlayer(int playerIndex);

    /*
    * Establece la penalización de tiempo (en ticks) aplicada al jugador por las mejoras compradas
    *
    * */
    void setPlayerTimePenaltyTicks(int playerId, int ticks);

    /*
    * Devuelve la penalización de tiempo (en ticks) aplicada al jugador por las mejoras compradas
    *
    */
    int getPlayerTimePenaltyTicks(int playerId);

    /*
    * Limpia las penalizaciones de tiempo de un jugador
    *
    * */
    void clearPlayerTimePenalties(int playerId);
};
#endif // SERVER_RACELOGIC_H
