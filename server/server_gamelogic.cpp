#include "server_gamelogic.h"
#include <iostream>
#include <cmath>


GameLogic::GameLogic() {
    world = raceBuilder.getWorld();
}


void GameLogic::processCommand(int car_id, const std::string& command, bool isPressed) {
    auto it = cars.find(car_id);
    if (it == cars.end()) {
        std::cout << "Comando recibido para auto desconocido ID " << car_id << std::endl;
        return;
    }
    auto& car = it->second;
    lastCommandPlayerId = car_id;

    if (command == "w") {
        car->setIsAccelerating(isPressed);
    } else if (command == "s") {
        car->setIsBraking(isPressed);
    } else if (command == "a") {
        car->setIsTurningLeft(isPressed);
    } else if (command == "d") {
        car->setIsTurningRight(isPressed);
    } else {
        std::cout << "Comando desconocido: " << command << std::endl; // deberias meter funcion de lectura de cheats
    }
}


void GameLogic::update() {
    const float dt = 1.0f / 60.0f; // Suponiendo 60 FPS
    for (auto const& [id, car] : cars) {
        car->applyMovement();
        car->applyFriction(); 
    }
    b2World_Step(world, dt, 4);
    checkCollisions(); // Verificar colisiones después de actualizar la física
}


std::shared_ptr<Snapshot> GameLogic::getSnapshot(EventType controlEvent) const {
    auto snapshot = std::make_shared<Snapshot>();
    snapshot->playerId = lastCommandPlayerId;
    snapshot->controlEvent = controlEvent;
    snapshot->playersSize = static_cast<uint32_t>(cars.size());

    for (auto const& [id, car] : cars) {
            CarStateDTO dto;
            dto.car_id = id;
            dto.health = car->getHealth();
            dto.position = car->getPosition();
            dto.angle = car->getDirection();

            snapshot->cars.push_back(dto);
    }
    return snapshot;
}


void GameLogic::addCar(int playerId, int carType) {
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