#include "server_racelogic.h"


void RaceLogic::addPlayer(int playerId) {
    nextCheckpointIndex[playerId] = 0;
    std::cout << "[RaceLogic] Jugador " << playerId << " agregado. Próximo checkpoint: 0" << std::endl;
}


bool RaceLogic::completedCheckpoint(int playerId, const Vector2D<float>& currentCarPosition, int& currentCheckpointIdx) {
    if (currentCheckpointIdx >= static_cast<int>(mapData.checkpoints.size())) {
        return false;
    }
    const Vector2D<float>& targetCheckpoint = mapData.checkpoints[currentCheckpointIdx];
    float dx = targetCheckpoint.x - currentCarPosition.x;
    float dy = targetCheckpoint.y - currentCarPosition.y;
    float distanceSq = dx*dx + dy*dy; 

    if (distanceSq <= (Constants::CHECKPOINT_RADIUS * Constants::CHECKPOINT_RADIUS)) {
        
        currentCheckpointIdx++;
        nextCheckpointIndex[playerId] = currentCheckpointIdx;
        
        if (currentCheckpointIdx < static_cast<int>(mapData.checkpoints.size())) {
            std::cout << "[RaceLogic] Jugador " << playerId << " cruzó checkpoint " 
                      << currentCheckpointIdx - 1 << ". Siguiente: " 
                      << std::to_string(currentCheckpointIdx) << std::endl;
        } else {
             std::cout << "[RaceLogic] Jugador " << playerId << " cruzó el ÚLTIMO checkpoint de la carrera." << std::endl;
        }
        return true;
    }
    return false;
}


bool RaceLogic::finishRace(int playerId) {

    std::lock_guard<std::mutex> lock(finishMutex);  // por si ambos terminan casi al mismo tiempo
     
    if (hasPlayerFinished(playerId)) {
        return false; 
    }

    auto it = nextCheckpointIndex.find(playerId);
   
    if (it == nextCheckpointIndex.end()) {
        return false;
    }

    if (it->second == static_cast<int>(mapData.checkpoints.size())) {
        finishedPlayers.push_back(playerId); // Guardamos el orden de llegada
        std::cout << "Jugador " << playerId << " ha terminado la carrera! Posición: " 
                  << finishedPlayers.size() << std::endl;
        return true;
    }
    return false;
}


bool RaceLogic::checkCheckpoint(int playerId, const Vector2D<float>& currentCarPosition) {
 
    auto it = nextCheckpointIndex.find(playerId);
    if (it == nextCheckpointIndex.end() || mapData.checkpoints.empty() || hasPlayerFinished(playerId)) {
        return false;
    }
    int currentCheckpointIdx = it->second;

    if (currentCheckpointIdx >= static_cast<int>(mapData.checkpoints.size())) {
        return false;
    }

    bool crossed = completedCheckpoint(playerId, currentCarPosition, currentCheckpointIdx);
    
    if (crossed && nextCheckpointIndex.at(playerId) == static_cast<int>(mapData.checkpoints.size())) {
        return finishRace(playerId); 
    }
    return false;
}


Vector2D<float> RaceLogic::getNextCheckpointPosition(int playerId) const {

    auto it = nextCheckpointIndex.find(playerId);
    
    if (it == nextCheckpointIndex.end() || mapData.checkpoints.empty()) {
        return Vector2D<float>{0.0f, 0.0f}; 
    }
    
    int currentCheckpointIdx = it->second;

    if (currentCheckpointIdx >= static_cast<int>(mapData.checkpoints.size())) {
        return mapData.checkpoints.back(); 
    }
    return mapData.checkpoints[currentCheckpointIdx];
}


bool RaceLogic::hasPlayerFinished(int playerId) const {
    return std::find(finishedPlayers.begin(), finishedPlayers.end(), playerId) != finishedPlayers.end();
}


std::vector<int> RaceLogic::getFinishedPlayers() const {
    return finishedPlayers;
}


std::vector<Vector2D<float>> RaceLogic::getHintsForPlayer(int playerId, const Vector2D<float>& currentCarPosition) const {
    
    std::vector<Vector2D<float>> hints;
    Vector2D<float> targetCheckpoint = getNextCheckpointPosition(playerId);
    
     if(hasPlayerFinished(playerId)) {
        return hints; 
    }

    // 2. Calcular el vector de dirección y la distancia total
    Vector2D<float> dir;
    dir.x = targetCheckpoint.x - currentCarPosition.x;
    dir.y = targetCheckpoint.y - currentCarPosition.y;
    float distance = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    
    if (distance < Constants::HINT_SPACING * 2 || distance == 0.0f) {
        return hints; 
    }

    Vector2D<float> unitDir;
    unitDir.x = dir.x / distance;
    unitDir.y = dir.y / distance; 

    float currentDistance = Constants::HINT_SPACING;
    
    // Fórmula: P_hint = P_inicial + (Dirección_Unitaria * Distancia)
    while (currentDistance < distance) {
        Vector2D<float> hintPos;
        hintPos.x = currentCarPosition.x + (unitDir.x * currentDistance);
        hintPos.y = currentCarPosition.y + (unitDir.y * currentDistance);
        
        hints.push_back(hintPos);
        currentDistance += Constants::HINT_SPACING;
    }
    return hints;
}
