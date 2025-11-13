#include "server_racelogic.h"



void RaceLogic::addPlayer(int playerId) {
    nextCheckpointIndex[playerId] = 0;
    std::cout << "[RaceLogic] Jugador " << playerId << " agregado. Próximo checkpoint: 0" << std::endl;
}


bool RaceLogic::completedCheckpoint(int playerId, const Vector2D<float>& currentCarPosition, int& currentCheckpointIdx) {
    if (currentCheckpointIdx >= mapData.checkpoints.size()) {
        return false;
    }
    const Vector2D<float>& targetCheckpoint = mapData.checkpoints[currentCheckpointIdx];
    float dx = targetCheckpoint.x - currentCarPosition.x;
    float dy = targetCheckpoint.y - currentCarPosition.y;
    float distanceSq = dx*dx + dy*dy; // Usamos distancia al cuadrado para evitar sqrt

    if (distanceSq <= (Constants::CHECKPOINT_RADIUS * Constants::CHECKPOINT_RADIUS)) {
        
        currentCheckpointIdx++;
        nextCheckpointIndex[playerId] = currentCheckpointIdx;
        
        if (currentCheckpointIdx <= mapData.checkpoints.size()) {
            std::cout << "[RaceLogic] Jugador " << playerId << " cruzó checkpoint " 
                      << currentCheckpointIdx - 1 << ". Siguiente: " 
                      << (currentCheckpointIdx < mapData.checkpoints.size() ? std::to_string(currentCheckpointIdx) : "Vuelta Completa") 
                      << std::endl;
        } 
        return true;
    }
    return false;
}


bool RaceLogic::checkCheckpoint(int playerId, const Vector2D<float>& currentCarPosition) {
 
    auto it = nextCheckpointIndex.find(playerId);
    int currentCheckpointIdx = it->second;

    if (mapData.checkpoints.empty()) {
        return false;
    }
    
    isLapCompleted(playerId, currentCheckpointIdx, currentCarPosition);
    bool crossed = completedCheckpoint(playerId, currentCarPosition, currentCheckpointIdx);
    
    if (crossed && currentCheckpointIdx >= mapData.checkpoints.size()) {
        return true; 
    }
    return false;
}


void RaceLogic::isLapCompleted(int playerId, int currentCheckpointIdx, const Vector2D<float>& currentCarPosition) {
    if (currentCheckpointIdx >= mapData.checkpoints.size()) {
        std::cout << "Jugador " << playerId << " ha completado una vuelta" << std::endl;
        currentCheckpointIdx = 0; // Reinicia el índice
        nextCheckpointIndex[playerId] = 0;
        completedLaps ++;
    }
}


Vector2D<float> RaceLogic::getNextCheckpointPosition(int playerId) const {
    auto it = nextCheckpointIndex.find(playerId);
    
    if (it == nextCheckpointIndex.end() || mapData.checkpoints.empty()) {
        return Vector2D<float>{0.0f, 0.0f}; 
    }
    
    int currentCheckpointIdx = it->second;

    if (currentCheckpointIdx >= mapData.checkpoints.size()) {
        return mapData.checkpoints[0];
    }
    return mapData.checkpoints[currentCheckpointIdx];
}


// revisar -> que pasa si entran dos al mismo tiempo al finish? tener en cuenta un lock
bool RaceLogic::finishRace(int playerId, int lapsToComplete) {
   
    if (hasPlayerFinished(playerId)) {
        return false; // Ya terminó, no procesar de nuevo
    }

    auto it = nextCheckpointIndex.find(playerId);
    if (it == nextCheckpointIndex.end()) {
        return false;
    }
    int currentCheckpointIdx = it->second;
    if (currentCheckpointIdx >= mapData.checkpoints.size() && completedLaps >= lapsToComplete) {
        std::cout << "Jugador " << playerId << " ha terminado la carrera!" << std::endl;
        finishedPlayers.push_back(playerId); // guardamos el orden de llegada
        completedLaps = 0; // reiniciamos para la proxima carrera
        return true;
    }
}


bool RaceLogic::hasPlayerFinished(int playerId) const {
    return std::find(finishedPlayers.begin(), finishedPlayers.end(), playerId) != finishedPlayers.end();
}


std::vector<int> RaceLogic::getFinishedPlayers() const {
    return finishedPlayers;
}
