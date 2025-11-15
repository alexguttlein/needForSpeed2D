#ifndef SERVER_RACELOGIC_H
#define SERVER_RACELOGIC_H

#include "../common/map.h"
#include "../common/vector2D.h"
#include "../common/constants.h"
#include "mapLoader.h"
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <mutex>


class RaceLogic {

private:
    MapLoader mapLoader;
    Map mapData = mapLoader.loadMapFromYaml("server/raceCheckpoints.yaml");
    std::map<int, int> nextCheckpointIndex;
    std::vector<int> finishedPlayers; // guardamos en orden los jugadores que terminaron
    std::mutex finishMutex;

public:
    /*
    * Constructor de RaceLogic
    *
    * */
    explicit RaceLogic() {}

    /*
    * Agrega un jugador al seguimiento de checkpoints
    *
    * */
    void addPlayer(int playerId);

    /*
    * Verifica si el jugador ha cruzado el checkpoint actual
    *
    * */    
    bool checkCheckpoint(int playerId, const Vector2D<float>& currentCarPosition);

    /*
    * Verifica si ha terminado la carrera
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
