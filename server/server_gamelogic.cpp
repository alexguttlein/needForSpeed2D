#include "server_gamelogic.h"


GameLogic::GameLogic(){
    world = raceBuilder.getWorld();
    auto objects = mapLoader.loadCollidersFromYaml("server/Mapa1-nfs.yaml");
    mapSetObjects.createBodiesFromObjects(world, objects);
    //loadStaticNpcs();
}


void GameLogic::processCommand(int car_id, const std::string& command, bool isPressed) {
    auto it = cars.find(car_id);
    if (it == cars.end()) {
        std::cout << "Comando recibido para auto desconocido ID " << car_id << std::endl;
        return;
    }
    auto& car = it->second;
    lastCommandPlayerId = car_id;

    if(raceState == IN_PROGRESS){
        if (command == Constants::MOVE_FORDWARD) {
            car->setIsAccelerating(isPressed);
        } else if (command == Constants::MOVE_BACKWARD) {
            car->setIsBraking(isPressed);
        } else if (command == Constants::TURN_LEFT) {
            car->setIsTurningLeft(isPressed);
        } else if (command == Constants::TURN_RIGHT) {
            car->setIsTurningRight(isPressed);
        }  else {
            std::cout << "Comando desconocido: " << command << std::endl; // deberias meter funcion de lectura de cheats
        }
    }
    if(raceState == WAITING_FOR_TRANSITION){        
        processUpgradeSelection(car_id, command);
    } 
}


void GameLogic::processUpgradeSelection(int car_id, const std::string& command){

    bool alreadySelected = hasSelectedUpgrade.count(car_id) && hasSelectedUpgrade[car_id];

    if (alreadySelected) {
        std::cout << "Jugador " << car_id << " ya selecciono una mejora para esta ronda." << std::endl;
        return;
    }

    int upgrade = Constants::DEFAULT_UPGRADE_ID;
    if (command == Constants::SELECT_HEALTH_UPGRADE) { upgrade = Constants::HEALTH_UPGRADE_ID; } 
    else if (command == Constants::SELECT_ACCELERATION_UPGRADE) { upgrade = Constants::ACCELERATION_UPGRADE_ID; }
    else if (command == Constants::SELECT_CONTROL_UPGRADE) { upgrade = Constants::CONTROL_UPGRADE_ID; }
    else if (command == Constants::SELECT_MAX_SPEED_UPGRADE) { upgrade = Constants::MAX_SPEED_UPGRADE_ID; }
    
    if (upgrade > Constants::DEFAULT_UPGRADE_ID) {
        hasSelectedUpgrade[car_id] = true;
        selectedUpgradeId[car_id] = upgrade;
        std::cout << "Jugador " << car_id << " selecciono MEJORA " << upgrade << "." << std::endl;
    }
}


void GameLogic::update(int currentTick) {
  
    if (raceState == GAME_OVER) {
        return; 
    }

    simulateRacePhysics(Constants::DT, currentTick);
    float currentRaceTime = getCurrentTimeSeconds(currentTick);  // con esto el timer se resetea al iniciar la carrera

    if (raceState == IN_PROGRESS) {
        simulateRaceInProgress(currentTick, currentRaceTime); 
    
    } else if (raceState == WAITING_FOR_TRANSITION) {
        simulateRaceInTransition(currentTick);
    }
}   


std::shared_ptr<Snapshot> GameLogic::getSnapshot(EventType controlEvent) const {
    auto snapshot = std::make_shared<Snapshot>();
    snapshot->playerId     = lastCommandPlayerId;
    snapshot->controlEvent = controlEvent;
    snapshot->playersSize  = static_cast<uint32_t>(cars.size());

    snapshot->raceStates.clear();
    snapshot->raceStates.reserve(cars.size());

    const auto& finishedPlayers = raceLogic.getFinishedPlayers();
    bool allFinished = !cars.empty();

    for (const auto& [id, car] : cars) {
        RaceStateDTO raceState{};
        raceState.playerId      = id;
        raceState.currentRaceId = raceLogic.getCurrentRaceId();
        raceState.nextCheckpoint = raceLogic.getNextCheckpointPosition(id);
        raceState.currentHints   = raceLogic.getHintsForPlayer(id, car->getPosition());
        raceState.hasFinished    = raceLogic.hasPlayerFinished(id);

        if (!raceState.hasFinished) {
            allFinished = false;
        }

        raceState.finishPosition = 0;
        raceState.finishTimeSeconds = raceLogic.getFinishTime(id);
        if (raceState.hasFinished) {
            auto it = std::find(finishedPlayers.begin(), finishedPlayers.end(), id);
            if (it != finishedPlayers.end()) {
                raceState.finishPosition = static_cast<int>(std::distance(finishedPlayers.begin(), it)) + 1;
            }
        }

        snapshot->raceStates.push_back(raceState);
    }

    snapshot->cars.clear();
    snapshot->cars.reserve(cars.size());
    for (auto const& [id, car] : cars) {
        CarStateDTO dto;
        dto.car_id   = id;
        dto.car_type_id = car->getCarType();
        dto.currentUpgradeId = car->getCurrentUpgradeId();
        dto.health   = car->getHealth();
        dto.position = car->getPosition();
        dto.angle    = car->getDirection();
        dto.speed    = car->getSpeed();       
        snapshot->cars.push_back(dto);
    }
    
    snapshot->raceFinished = allFinished;

    if(raceState == GAME_OVER){
        snapshot->gameFinished = true;
        snapshot->leaderboards = leaderboard;
    }

    return snapshot;
}


void GameLogic::addCar(int playerId, int carType) {
    raceLogic.addPlayer(playerId); // Agregar jugador a RaceLogic
    raceBuilder.addSelectCar(carType);
    std::shared_ptr<Car> newCar = raceBuilder.getCars().back();
    cars[playerId] = newCar;
   
}


void GameLogic::loadStaticNpcs() {
    npcsData = YamlLoader::loadNPCsFromYaml("server/npcs.yaml"); 
    for (const auto& npc : npcsData) {
        raceBuilder.addStaticNpcCar(npc.carType, Vector2D<float>{npc.x, npc.y}); // podriamos pasar el angulo? ver despues
         std::shared_ptr<Car> npcCar = raceBuilder.getStaticNpcs().back();
        staticNpcs[npc.id] = npcCar;
    }
}


void GameLogic::checkCollisions() {
    const b2ContactEvents contactEvents = b2World_GetContactEvents(world);

    if (contactEvents.beginCount > 0) {
        for (int i = 0; i < contactEvents.beginCount; ++i) {
            const b2ContactBeginTouchEvent* event = &contactEvents.beginEvents[i];
            
            b2BodyId bodyA = b2Shape_GetBody(event->shapeIdA);
            b2BodyId bodyB = b2Shape_GetBody(event->shapeIdB);

            Car* carA = static_cast<Car*>(b2Shape_GetUserData(event->shapeIdA));
            Car* carB = static_cast<Car*>(b2Shape_GetUserData(event->shapeIdB));

            if (carA || carB) {
                 
                float hitSpeed = getCollisionSpeed(bodyA, bodyB);

                const float MIN_HIT_SPEED = 1.0f; 
                if (hitSpeed < MIN_HIT_SPEED) {
                    continue; // No es un impacto severo, ignorar
                }
                
                b2Vec2 normal = getCollisionNormal(bodyA, bodyB);
                applyCollisionDamage(carA, carB, normal, hitSpeed);

                std::cout << "💥 TOUCH DETECTADO (Aprox). Vida A: " 
                          << (carA ? std::to_string(carA->getHealth()) : "N/A") 
                          << ", Vida B: " 
                          << (carB ? std::to_string(carB->getHealth()) : "N/A") 
                          << ", Velocidad: " << hitSpeed
                          << std::endl;       
            }    
        }
    }
    if (contactEvents.endCount > 0) {
        for (int i = 0; i < contactEvents.endCount; ++i) {
            const b2ContactEndTouchEvent* event = &contactEvents.endEvents[i];
            std::cout << "✅ CONTACTO FINALIZADO entre "
                      << b2Shape_GetBody(event->shapeIdA).index1
                      << " y " << b2Shape_GetBody(event->shapeIdB).index1
                      << std::endl;
        }
    }
}


void GameLogic::applyCollisionDamage(Car* carA, Car* carB, b2Vec2 normal, float hitSpeed) {
    const float DAMAGE_FACTOR = 0.8f; 
    float damage = std::pow(hitSpeed, 1.3f) * DAMAGE_FACTOR;

    if (carA) {
        Vector2D<float> forwardA = carA->getDirection();
        float angleFactorA = std::fabs(forwardA.x * normal.x + forwardA.y * normal.y);
        float finalDamageA = damage * angleFactorA;
        carA->takeDamage(finalDamageA);
    }
    if (carB) {
        Vector2D<float> forwardB = carB->getDirection();
        b2Vec2 inverseNormal = b2Neg(normal); 
        float angleFactorB = std::fabs(forwardB.x * inverseNormal.x + forwardB.y * inverseNormal.y);
        float finalDamageB = damage * angleFactorB;
        carB->takeDamage(finalDamageB);
    }
}


b2Vec2 GameLogic::getCollisionNormal(b2BodyId bodyA, b2BodyId bodyB) {
    b2Vec2 posA = b2Body_GetPosition(bodyA);
    b2Vec2 posB = b2Body_GetPosition(bodyB);
    b2Vec2 normal = b2Normalize(b2Sub(posB, posA)); 
    return normal;
}


float GameLogic::getCollisionSpeed(b2BodyId bodyA, b2BodyId bodyB) {
    b2Vec2 velA = b2Body_GetLinearVelocity(bodyA);
    b2Vec2 velB = b2Body_GetLinearVelocity(bodyB);
    b2Vec2 relativeVel = b2Sub(velA, velB); 
    return b2Length(relativeVel);
}


void GameLogic::simulateRaceInTransition(int currentTick){
    if (currentTick - transitionStartTick >= Constants::UPGRADE_WAIT_TICKS) {
            
            std::cout << "--- TRANSICIÓN: CONFIGURANDO PRÓXIMA CARRERA ---" << std::endl;
            
            resetFinishRace();
            applyUpgradeToCar();
            hasSelectedUpgrade.clear();
            selectedUpgradeId.clear();

            raceStartTick = currentTick;
            raceState = IN_PROGRESS; 
    } 
}


void GameLogic::applyUpgradeToCar(){

    for (auto const& [id, upgradeId] : selectedUpgradeId) {
        auto carIt = cars.find(id);
        if (carIt != cars.end() && upgradeId > Constants::DEFAULT_UPGRADE_ID) {

            std::shared_ptr<Car> car = carIt->second;
            car->applyUpgrade(upgradeId);

            float penalizeTime = Constants::DEFAULT_PENALIZE;
            switch (upgradeId) {
                case Constants::HEALTH_UPGRADE_ID:
                    penalizeTime = Constants::PENALIZE_HEALTH_UPGRADE;
                    break;
                case Constants::ACCELERATION_UPGRADE_ID:
                    penalizeTime = Constants::PENALIZE_ACCELERATION_UPGRADE;
                    break;
                case Constants::CONTROL_UPGRADE_ID:
                    penalizeTime = Constants::PENALIZE__CONTROL_UPGRADE;
                    break;
                case Constants::MAX_SPEED_UPGRADE_ID:
                    penalizeTime = Constants::PENALIZE_SPEED_UPGRADE;
                    break;
                default:
                    penalizeTime = Constants::DEFAULT_PENALIZE;
                    break;
            }
            raceLogic.upgradePenalizeTimeToPlayer(penalizeTime, id); // penalizo tiempo por mejora
            std::cout << "Penalizando al jugador " << id << " con " << penalizeTime << " segundos por mejora." << std::endl;
            std::cout << "Aplicando MEJORA " << upgradeId << " al jugador " << id << std::endl; 
        }
    }
}


void GameLogic::resetFinishRace(){

    std::vector<int> finishedPlayers = raceLogic.getFinishedPlayers(); // obtenemos orden de llegada
    raceLogic.setCurrentRace(); 
    std::vector<Vector2D<float>> checkpoints = raceLogic.getActualRaceCheckpoints();
    
    if (!checkpoints.empty()) {
        const Vector2D<float>& newSpawnPoint = checkpoints[0];
        raceBuilder.setBaseSpawnPoint(newSpawnPoint);

        // 1. REPOSICIONAR: (orden de llegada)
        for (int playerId : finishedPlayers) {
            auto carIt = cars.find(playerId);
            if (carIt != cars.end()) {
                std::shared_ptr<Car> car = carIt->second;
                
                Vector2D<float> spawnPos = raceLogic.getSpawnPositionForPlayer(playerId);
                car->resetMovementStates();
                car->setPosition(spawnPos); 
                car->setRotationToRight();
                car->resetVelocity(); 
                car->clearUpgradeEffects();
                raceLogic.addPlayer(playerId); 
            }
        }
        resetNpcs();
        resetRaceTemporizer(); // Resetea el temporizador para la siguiente carrera
    }
}


void GameLogic::resetNpcs(){
    for (const auto& npc : npcsData) {
        auto it = staticNpcs.find(npc.id);
        if (it != staticNpcs.end()) {
            std::shared_ptr<Car> npcCar = it->second;
            Vector2D<float> spawnPos{npc.x, npc.y};
            npcCar->setPosition(spawnPos); 
            npcCar->resetVelocity(); 
        }
    }
}


void GameLogic::simulateRaceInProgress(int currentTick, float currentRaceTime) {

    for (auto const& [id, car] : cars) {
    
        if (!raceLogic.hasPlayerFinished(id)) {
            Vector2D<float> carPosition = car->getPosition();
            bool justFinished = raceLogic.checkCheckpoint(id, carPosition); 
            if (justFinished) {
                raceLogic.setCurrentRaceTimeSeconds(currentRaceTime);
                raceLogic.addTimeFinishPlayer(currentRaceTime, id); // actualizo tiempo en carrera total
            }
        }
    }
    if (raceLogic.isRaceOver()) {

        if(raceLogic.hasNextRace()) {
            setTransition(currentTick);
        }
        else {
            finishGame();
        }
    } 
}


void GameLogic::simulateRacePhysics(const float dt, int currentTick) {

    for (auto const& [id, car] : cars) {
        car->applyMovement();
        car->applyFriction(); 
    }
    checkFinishRaceByTime(currentTick);
    b2World_Step(world, dt, 4);
    checkCollisions(); // Verificar colisiones después de actualizar la física
}


float GameLogic::getCurrentTimeSeconds(int currentTick) {
    int ticksElapsed = currentTick - raceStartTick;
    return static_cast<float>(ticksElapsed) / static_cast<float>(Constants::TICKS_PER_SECOND);
}


void GameLogic::setTransition(int currentTick) {
    std::cout << "--- CARRERA TERMINADA. INICIANDO ESPERA de " << Constants::UPGRADE_WAIT_SECONDS << " segundos ---" << std::endl;
    raceState = WAITING_FOR_TRANSITION;
    transitionStartTick = currentTick;
}


void GameLogic::finishGame() {
    std::cout << "--- CARRERA TERMINADA. NO HAY MÁS CIRCUITOS. ---" << std::endl;
    leaderboard = raceLogic.getLeaderBoard();

    std::cout << "🏆 LEADERBOARD FINAL 🏆" << std::endl;
    for (size_t i = 0; i < leaderboard.size(); ++i) {
        const auto& entry = leaderboard[i];
        std::cout << (i + 1) << ". Jugador " << entry.playerId 
                << " - Tiempo Total: " << entry.finishTime << " segundos." << std::endl;
    
    }
    raceState = GAME_OVER;
}


void GameLogic::checkFinishRaceByTime(int currentTick) {
    
    if (lastTickChecked == 0 || currentTick < lastTickChecked) {
        lastTickChecked = currentTick;
        return; 
    }

    int ticksPassed = currentTick - lastTickChecked;
    lastTickChecked = currentTick;

    if (remainingFinishTicks > 0) {
        remainingFinishTicks -= ticksPassed;
    }
    
    if(remainingFinishTicks <= 0){
        std::cout << "Tiempo máximo de la carrera alcanzado. Finalizando la carrera..." << std::endl;
        finishGame(); 
    }
}


void GameLogic::resetRaceTemporizer() {
    remainingFinishTicks = Constants::MAX_TICKS;
    lastTickChecked = 0;
}


GameLogic::~GameLogic() {

    // 1. Destruir bodies ANTES del world
    for (auto& [id, car] : cars) {
        if (car) car->destroyBody();
    }
    cars.clear();

    // 2. Ahora destruir el world
    if (b2World_IsValid(world)) {
        b2DestroyWorld(world);
        world = b2WorldId{};
    }
}
