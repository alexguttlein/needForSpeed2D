#include "server_gamelogic.h"


GameLogic::GameLogic(){
    world = raceBuilder.getWorld();
    //auto objects = mapLoader.loadCollidersFromYaml("server/Mapa1-nfs.yaml");
    //mapSetObjects.createBodiesFromObjects(world, objects);

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
        if (command == "w") {
            car->setIsAccelerating(isPressed);
        } else if (command == "s") {
            car->setIsBraking(isPressed);
        } else if (command == "a") {
            car->setIsTurningLeft(isPressed);
        } else if (command == "d") {
            car->setIsTurningRight(isPressed);
        }  else {
            std::cout << "Comando desconocido: " << command << std::endl; // deberias meter funcion de lectura de cheats
        }
    }


    if(raceState == WAITING_FOR_TRANSITION){
        
        bool alreadySelected = hasSelectedUpgrade.count(car_id) && hasSelectedUpgrade[car_id];

        if (alreadySelected) {
            std::cout << "Jugador " << car_id << " ya selecciono una mejora para esta ronda." << std::endl;
            return;
        }

        int upgrade = 0;
        if (command == "1") { upgrade = 1; } 
        else if (command == "2") { upgrade = 2; }
        else if (command == "3") { upgrade = 3; }
        else if (command == "4") { upgrade = 4; }
        
        if (upgrade > 0) {
            // 2. Marcar el estado como TRUE de forma PERSISTENTE para este jugador
            hasSelectedUpgrade[car_id] = true;
            selectedUpgradeId[car_id] = upgrade;
            std::cout << "Jugador " << car_id << " selecciono MEJORA " << upgrade << "." << std::endl;
        }

    } 

}


void GameLogic::update(int currentTick) {
    const float dt = 1.0f / 60.0f;


    if (raceState == GAME_OVER) {
        return; 
    }


    for (auto const& [id, car] : cars) {
        car->applyMovement();
        car->applyFriction(); 
    }

    // agregar funcion de fin de juego por tiempo
    if (currentTick >= Constants::MAX_TICKS) {
        std::cout << "Tiempo máximo de la partida alcanzado. Finalizando juego..." << std::endl;
    }

    b2World_Step(world, dt, 4);
    checkCollisions(); // Verificar colisiones después de actualizar la física

    // con esto el timer se resetea al iniciar la carrera
    int ticksElapsed = currentTick - raceStartTick;
    float currentRaceTime = static_cast<float>(ticksElapsed) /
                            static_cast<float>(Constants::TICKS_PER_SECOND);


    if (raceState == IN_PROGRESS) {
        for (auto const& [id, car] : cars) {
            if (!raceLogic.hasPlayerFinished(id)) {
                Vector2D<float> carPosition = car->getPosition();
                bool justFinished = raceLogic.checkCheckpoint(id, carPosition); 
                if (justFinished) {
                    raceLogic.setCurrentRaceTimeSeconds(currentRaceTime);
                    raceLogic.addTimeFinishPlayer(currentRaceTime, id); // actualizo tiempo en carrera total
                    //checkeo timer general
                    float alltime = raceLogic.getAllTimeFinishTime(id);
                    std::cout << "Jugador " << id << " tiempo general hasta ahora: " 
                              << alltime << " segundos." << std::endl;
                }
            }
        }
        
        if (raceLogic.isRaceOver()) {
            std::cout << "--- CARRERA TERMINADA. INICIANDO ESPERA de " 
                      << Constants::UPGRADE_WAIT_SECONDS << " segundos ---" << std::endl;
            
            raceState = WAITING_FOR_TRANSITION;
            transitionStartTick = currentTick;
           
        }
    } 


    else if (raceState == WAITING_FOR_TRANSITION) {
        
        if (currentTick - transitionStartTick >= Constants::UPGRADE_WAIT_TICKS) {
            
            if (raceLogic.hasNextRace()) {
                std::cout << "--- TRANSICIÓN: CONFIGURANDO PRÓXIMA CARRERA ---" << std::endl;
                
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
                            
                            Vector2D<float> spawnPos = raceBuilder.getSpawnPosition(); 
                            // Reposicionar, resetear y añadir a la nueva carrera
                            car->resetMovementStates();
                            car->setPosition(spawnPos); 
                            car->resetVelocity(); 
                            raceLogic.addPlayer(playerId); 
                        }
                    }
                }


                // Aplicar mejoras leyendo la INTENCIÓN GUARDADA
                for (auto const& [id, upgradeId] : selectedUpgradeId) {
                    auto carIt = cars.find(id);
                    if (carIt != cars.end() && upgradeId > 0) {
                        std::shared_ptr<Car> car = carIt->second;
                        // Aquí llamas a la función que aplica el efecto real al Car.
                        //car->applyUpgrade(upgradeId);
                        std::cout << "Aplicando MEJORA " << upgradeId << " al jugador " << id << std::endl; 
                    }
                }
                hasSelectedUpgrade.clear();
                selectedUpgradeId.clear();

                raceStartTick = currentTick;
                raceState = IN_PROGRESS; 
                
            } else {
                // FIN DE LA COMPETICIÓN
                std::cout << "--- COMPETICIÓN FINALIZADA. NO HAY MÁS CIRCUITOS. ---" << std::endl;
                raceState = GAME_OVER; 
            }
        } 
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
        dto.health   = car->getHealth();
        dto.position = car->getPosition();
        dto.angle    = car->getDirection();
        dto.speed    = car->getSpeed();
        snapshot->cars.push_back(dto);
    }

    snapshot->raceFinished = allFinished;

    return snapshot;
}


void GameLogic::addCar(int playerId, int carType) {
    raceLogic.addPlayer(playerId); // Agregar jugador a RaceLogic
    raceBuilder.addSelectCar(carType);
    std::shared_ptr<Car> newCar = raceBuilder.getCars().back();
    cars[playerId] = newCar;
   
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

                const float MIN_HIT_SPEED = 0.2f; 
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
    const float DAMAGE_FACTOR = 1.5f; 
    float damage = (hitSpeed * hitSpeed) * DAMAGE_FACTOR;

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
