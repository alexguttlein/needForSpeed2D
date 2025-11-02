#include "server_gamelogic.h"
#include <iostream>
#include <cmath>


GameLogic::GameLogic() {
    // Inicialización del mundo Box2D (aún vacía, se llenará después)
    // world = std::make_unique<b2World>(b2Vec2(0.0f, 0.0f)); // Ejemplo para el futuro
}


// // por ahora, no deberia ser asi.
// void GameLogic::processCommand(uint8_t car_id, const std::string& command) {
//     auto it = cars.find(car_id);
//     if (it != cars.end()) {
//         auto& car = it->second;

//         if (command == "ACCELERATE") {
//             car->accelerate();
//         } else if (command == "BREAK_REVERSE") {
//             car->breakReverse();
//         } else if (command == "TURN_LEFT") {
//             car->turnLeft();
//         } else if (command == "TURN_RIGHT") {
//             car->turnRight();
//         }
//     } else {
//         std::cout << "Comando recibido para auto desconocido ID " << (int)car_id << std::endl;
//     }
// }

void GameLogic::update(float dt) {

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