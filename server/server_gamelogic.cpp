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