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

        b2Vec2 posA = b2Body_GetPosition(bodyA);
        b2Vec2 posB = b2Body_GetPosition(bodyB);

        std::cout << "🚗💥 CONTACTO INICIADO entre cuerpos "
                  << bodyA.index1 << " y " << bodyB.index1
                  << " en posiciones (" << posA.x << ", " << posA.y 
                  << ") y (" << posB.x << ", " << posB.y << ")" << std::endl;

        // ⚙️ Hack suave: "despegar" apenas el cuerpo, no mandarlo lejos
        // Esto resetea el contacto sin romper la física del mundo.
        const float epsilon = 0.001f;
        b2Vec2 smallShift = {epsilon, epsilon};

        // Mover apenas el cuerpo A
        b2Vec2 shiftedPos = {posA.x + smallShift.x, posA.y + smallShift.y};
        b2Rot rotA = b2Body_GetRotation(bodyA);

        b2Body_SetTransform(bodyA, shiftedPos, rotA);
        b2Body_SetTransform(bodyA, posA, rotA);
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