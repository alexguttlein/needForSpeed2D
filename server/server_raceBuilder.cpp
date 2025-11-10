#include "server_raceBuilder.h"


RaceBuilder::RaceBuilder() : nextSpawnX(Constants::SPAWN_START_X), nextSpawnY(Constants::SPAWN_START_Y){

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{0.0f, 0.0f}; 
    world = b2CreateWorld(&worldDef);
}


RaceBuilder::~RaceBuilder() {
        b2DestroyWorld(world);
}


// por ahora asi, mas adelante se deberia poner a cada auto segun como salio en la carrera
Vector2D<float> RaceBuilder::getNextSpawnPosition() {
    Vector2D<float> currentPos = {nextSpawnX, nextSpawnY};
    nextSpawnX += Constants::SPAWN_OFFSET_X;

    if (nextSpawnX > Constants::MAX_ROW_X) {
        nextSpawnX = Constants::SPAWN_START_X;
        nextSpawnY += Constants::SPAWN_OFFSET_X;
    }



    return currentPos;
}


// por ahora hardcodeamos 3 tipos de autos -> deberian estar hardcodeados en algun lado
void RaceBuilder::addSelectCar(int carType) {
    Vector2D<float> position = getNextSpawnPosition();
    std::shared_ptr<Car> car;
    switch (carType) {
        case 1:
            car = std::make_shared<Car>(world, Vector2D<float>{position.x / Constants::SCALE_METER_TO_PIXEL, position.y / Constants::SCALE_METER_TO_PIXEL}, 100.0f, 40.0f, 6.0f, 200.0f, 50.0f, 100.0f);
            break;
        case 2:
            car = std::make_shared<Car>(world, Vector2D<float>{position.x, position.y}, 200.0f, 250.0f, 40.0f, 300.0f, 40.0f, 150.0f);
            break;
        case 3:
            car = std::make_shared<Car>(world, Vector2D<float>{position.x, position.y}, 120.0f, 180.0f, 40.0f, 200.0f, 100.0f, 120.0f);
            break;
        default:
            std::cerr << "Tipo de auto desconocido: " << carType << std::endl;
            return;
    }
    cars.push_back(car);
}


b2WorldId RaceBuilder::getWorld() const {
    return world;
}


std::vector<std::shared_ptr<Car>>& RaceBuilder::getCars() {
    return cars;
}
