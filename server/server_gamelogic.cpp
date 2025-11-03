#include "server_gamelogic.h"
#include <iostream>
#include <cmath>


GameLogic::GameLogic() {
    // Inicialización del mundo Box2D (aún vacía, se llenará después)
    // world = std::make_unique<b2World>(b2Vec2(0.0f, 0.0f)); // Ejemplo para el futuro

    Vector2D<float> pos(0.0f, 0.0f);
    float acceleration = 0.5f;
    float control = 0.04f;
    float weight = 1200.0f;
    float maxSpeed = 20.0f;
    float maxReverseSpeed = 6.0f;
    float health = 100.0f;

    auto testCar = std::make_shared<Car>(pos, acceleration, control, weight, maxSpeed, maxReverseSpeed, health);
    cars.emplace(0, testCar);
    std::cout << "GameLogic: created test car id 0\n";
}



void GameLogic::processCommand(int car_id, const std::string& command) {
    auto it = cars.find(car_id);
    if (it == cars.end()) {
        std::cout << "Comando recibido para auto desconocido ID " << car_id << std::endl;
        return;
    }
    auto& car = it->second;

    if (command == "w") {
        car->accelerate();
    } else if (command == "s") {
        car->breakReverse();
    } else if (command == "a") {
        car->turnLeft();
    } else if (command == "d") {
        car->turnRight();
    } else {
        std::cout << "Comando desconocido: " << command << std::endl;
    }
}


void GameLogic::update(float dt) {
    for (auto const& [id, car] : cars) {
        car->updatePosition(dt);
    }
}


GameSnapshot GameLogic::getSnapshot() const {
    GameSnapshot snapshot = {};
    snapshot.num_cars = 0;

    for (auto const& [id, car] : cars) {
        if (id < 8) { 
            CarStateDTO dto;
            
            // LECTURA DE DATOS DEL AUTO
            dto.car_id = id;
            dto.health = car->getHealth();
            dto.position = car->getPosition();
            dto.angle = car->getDirection();

            snapshot.car_states[snapshot.num_cars++] = dto;
        }
    }
    return snapshot;
}