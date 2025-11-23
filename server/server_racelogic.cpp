#include "server_racelogic.h"


RaceLogic::RaceLogic() : 
    yamlLoader(),
    mapData(yamlLoader.loadMapFromYaml("server/raceCheckpoints.yaml")),
    actualRaceId("race_1"),
    currentRaceId(1) {
    
        setCurrentCheckpoints(actualRaceId);
}


std::vector<Vector2D<float>> RaceLogic::getActualRaceCheckpoints() {
    return actualRaceCheckpoints;
}


void RaceLogic::setCurrentRace() {
    
    std::cout << "tiempo que hizo en la carrera: " << std::endl;
    for (int playerId : finishedPlayers) {
        std::cout << "Jugador " << playerId << ": " << finishTimes[playerId] << " segundos." << std::endl;
    }
    finishedPlayers.clear();
    nextCheckpointIndex.clear();
    finishTimes.clear();
    currentRaceId++;
    actualRaceId = "race_" + std::to_string(currentRaceId);
    std::cout << "[RaceLogic] Configurando el circuito de carrera actual: " << actualRaceId << std::endl;

    setCurrentCheckpoints(actualRaceId);
}


bool RaceLogic::hasNextRace() {
    std::string nextRaceId = "race_" + std::to_string(currentRaceId + 1);
    return mapData.circuits.find(nextRaceId) != mapData.circuits.end();
} 

bool RaceLogic::isRaceOver() {    
    if (nextCheckpointIndex.empty()) {
        return true; 
    }
    
    int activePlayers = getActiveRacePlayers();
    int requiredToFinish = (activePlayers / 2) + 1;

    if (activePlayers == 1) {
        requiredToFinish = 1;
    }
    return static_cast<int>(finishedPlayers.size()) >= requiredToFinish;
}


void RaceLogic::setCurrentCheckpoints(std::string raceId) {
    auto it = mapData.circuits.find(raceId);
    if (it != mapData.circuits.end()) {
        actualRaceCheckpoints = it->second.checkpoints;
        std::cout << "[RaceLogic] Circuito cargado con " << actualRaceCheckpoints.size() << " checkpoints." << std::endl;
    } else {
        std::cerr << "[RaceLogic ERROR] Circuito con ID '" << actualRaceId << "' no encontrado en los datos del mapa." << std::endl;
    }
}


void RaceLogic::addPlayer(int playerId) {
    nextCheckpointIndex[playerId] = 0;
    std::cout << "[RaceLogic] Jugador " << playerId << " agregado. Próximo checkpoint: 0" << std::endl;
}


void RaceLogic::removePlayer(int playerId) {
    nextCheckpointIndex.erase(playerId);
}


int RaceLogic::getActiveRacePlayers() {
    return static_cast<int>(nextCheckpointIndex.size());
}


bool RaceLogic::completedCheckpoint(int playerId, const Vector2D<float>& currentCarPosition, int& currentCheckpointIdx) {
    if (currentCheckpointIdx >= static_cast<int>(actualRaceCheckpoints.size())) {
        return false;
    }
    const Vector2D<float>& targetCheckpoint = actualRaceCheckpoints[currentCheckpointIdx];
    float dx = targetCheckpoint.x - currentCarPosition.x;
    float dy = targetCheckpoint.y - currentCarPosition.y;
    float distanceSq = dx*dx + dy*dy; 

    if (distanceSq <= (Constants::CHECKPOINT_RADIUS * Constants::CHECKPOINT_RADIUS)) {
        
        currentCheckpointIdx++;
        nextCheckpointIndex[playerId] = currentCheckpointIdx;
        
        if (currentCheckpointIdx < static_cast<int>(actualRaceCheckpoints.size())) {
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

    if (it->second == static_cast<int>(actualRaceCheckpoints.size())) {
        finishedPlayers.push_back(playerId); // Guardamos el orden de llegada
        if (finishTimes.find(playerId) == finishTimes.end()) {
            finishTimes[playerId] = 0.0f;
        }
        std::cout << "Jugador " << playerId << " ha terminado la carrera! Posición: "
                  << finishedPlayers.size() << std::endl;
        return true;
    }
    return false;
}


bool RaceLogic::checkCheckpoint(int playerId, const Vector2D<float>& currentCarPosition) {
 
    auto it = nextCheckpointIndex.find(playerId);
    if (it == nextCheckpointIndex.end() || actualRaceCheckpoints.empty() || hasPlayerFinished(playerId)) {
        return false;
    }
    int currentCheckpointIdx = it->second;

    if (currentCheckpointIdx >= static_cast<int>(actualRaceCheckpoints.size())) {
        return false;
    }

    bool crossed = completedCheckpoint(playerId, currentCarPosition, currentCheckpointIdx);
    
    if (crossed && nextCheckpointIndex.at(playerId) == static_cast<int>(actualRaceCheckpoints.size())) {
        return finishRace(playerId); 
    }
    return false;
}


Vector2D<float> RaceLogic::getNextCheckpointPosition(int playerId) const {

    auto it = nextCheckpointIndex.find(playerId);
    
    if (it == nextCheckpointIndex.end() || actualRaceCheckpoints.empty()) {
        return Vector2D<float>{0.0f, 0.0f}; 
    }
    
    int currentCheckpointIdx = it->second;

    if (currentCheckpointIdx >= static_cast<int>(actualRaceCheckpoints.size())) {
        return actualRaceCheckpoints.back(); 
    }
    return actualRaceCheckpoints[currentCheckpointIdx];
}


bool RaceLogic::hasPlayerFinished(int playerId) const {
    return std::find(finishedPlayers.begin(), finishedPlayers.end(), playerId) != finishedPlayers.end();
}


std::vector<int> RaceLogic::getFinishedPlayers() const {
    return finishedPlayers;
}


float RaceLogic::getFinishTime(int playerId) const {
    auto it = finishTimes.find(playerId);
    if (it == finishTimes.end()) {
        return -1.0f;
    }
    return it->second;
}


void RaceLogic::setCurrentRaceTimeSeconds(float currentTimeSeconds) {
    std::lock_guard<std::mutex> lock(finishMutex);
    for (int playerId : finishedPlayers) {
        auto it = finishTimes.find(playerId);
        if (it != finishTimes.end() && it->second <= 0.0f) {
            it->second = currentTimeSeconds;
        }
    }
}


void RaceLogic::addTimeFinishPlayer(float addedTime, int playerId) {
    std::lock_guard<std::mutex> lock(finishMutex);
    auto it = allTimeFinishTimes.find(playerId);
    if (it != allTimeFinishTimes.end()) {
        it->second += addedTime;
    } else {
        allTimeFinishTimes[playerId] = addedTime;
    }
  
}


void RaceLogic::upgradePenalizeTimeToPlayer(float penalizeTime, int playerId) {
    std::lock_guard<std::mutex> lock(finishMutex);
    auto it = allTimeFinishTimes.find(playerId);
    if (it != allTimeFinishTimes.end()) {
        it->second += penalizeTime;
    } 
}


float RaceLogic::getAllTimeFinishTime(int playerId) {
    auto it = allTimeFinishTimes.find(playerId);
    if (it == allTimeFinishTimes.end()) {
        return -1.0f;
    }
    return it->second;
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


int RaceLogic::getCurrentRaceId() const {
    return currentRaceId;
}


Leaderboard RaceLogic::getLeaderBoard() const {
    
    Leaderboard leaderboard;

    for (const auto& pair : allTimeFinishTimes) {
        float time = pair.second;
        
        if (time >= 0.0f) { 
            leaderboard.push_back({pair.first, time});
        }
    }

    auto compareByTime = [](const PlayerTime& a, const PlayerTime& b) {
        return a.finishTime < b.finishTime; // Orden ascendente
    };

    std::sort(leaderboard.begin(), leaderboard.end(), compareByTime);
    return leaderboard;
}
