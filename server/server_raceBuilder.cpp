#include "server_raceBuilder.h"


RaceBuilder::RaceBuilder() : nextSpawnX(Constants::SPAWN_START_X), nextSpawnY(Constants::SPAWN_START_Y){

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{0.0f, 0.0f}; 
    worldDef.hitEventThreshold = 0.1f;
    world = b2CreateWorld(&worldDef);
}


RaceBuilder::~RaceBuilder() {
    // b2DestroyWorld(world);
}


Vector2D<float> RaceBuilder::getSpawnPosition() {
    Vector2D<float> currentPos = {nextSpawnX, nextSpawnY};
    nextSpawnX += Constants::SPAWN_OFFSET_X;

    if (nextSpawnX > Constants::MAX_ROW_X) {
        nextSpawnX = Constants::SPAWN_START_X;
        nextSpawnY += Constants::SPAWN_OFFSET_X;
    }
    currentPos.x /= Constants::SCALE_METER_TO_PIXEL;
    currentPos.y /= Constants::SCALE_METER_TO_PIXEL;
    return currentPos;
}


void RaceBuilder::setBaseSpawnPoint(const Vector2D<float>& basePoint) {

    baseSpawnX = basePoint.x * Constants::SCALE_METER_TO_PIXEL;
    baseSpawnY = basePoint.y * Constants::SCALE_METER_TO_PIXEL;
    
    nextSpawnX = baseSpawnX;
    nextSpawnY = baseSpawnY;
    
    nextSpawnX += Constants::SPAWN_START_X;
    nextSpawnY += Constants::SPAWN_START_Y;
}




void RaceBuilder::addSelectCar(int carType) {
    Vector2D<float> position = getSpawnPosition();
    std::shared_ptr<Car> car;
    switch (carType) {

        // Atributos: world, position, carType, acceleration, control, weight, maxSpeed, maxReverseSpeed, health, height, width

        case Constants::FORD:
            car = std::make_shared<Car>(world, position, carType, Constants::FORD_BASE_ACCELERATION,
                                        Constants::FORD_BASE_CONTROL, Constants::FORD_BASE_WEIGHT,
                                        Constants::FORD_BASE_MAX_SPEED, Constants::FORD_BASE_REVERSE_SPEED,
                                        Constants::FORD_BASE_HEALTH, Constants::FORD_HEIGHT_SPRITE, Constants::FORD_WIDTH_SPRITE);
            break;
        case Constants::MAZDA:
            car = std::make_shared<Car>(world, position, carType, Constants::MAZDA_BASE_ACCELERATION,
                                        Constants::MAZDA_BASE_CONTROL, Constants::MAZDA_BASE_WEIGHT, 
                                        Constants::MAZDA_BASE_MAX_SPEED, Constants::MAZDA_BASE_REVERSE_SPEED,
                                        Constants::MAZDA_BASE_HEALTH, Constants::MAZDA_HEIGHT_SPRITE, Constants::MAZDA_WIDTH_SPRITE);
            break;
        case Constants::CORROLLA:
            car = std::make_shared<Car>(world, position, carType, Constants::CORROLLA_BASE_ACCELERATION,
                                        Constants::CORROLLA_BASE_CONTROL, Constants::CORROLLA_BASE_WEIGHT,
                                        Constants::CORROLLA_BASE_MAX_SPEED, Constants::CORROLLA_BASE_REVERSE_SPEED,
                                        Constants::CORROLLA_BASE_HEALTH, Constants::CORROLLA_HEIGHT_SPRITE, Constants::CORROLLA_WIDTH_SPRITE);
            break;
        case Constants::BMW:
            car = std::make_shared<Car>(world, position, carType, Constants::BMW_BASE_ACCELERATION,
                                        Constants::BMW_BASE_CONTROL, Constants::BMW_BASE_WEIGHT,
                                        Constants::BMW_BASE_MAX_SPEED, Constants::BMW_BASE_REVERSE_SPEED,
                                        Constants::BMW_BASE_HEALTH, Constants::BMW_HEIGHT_SPRITE, Constants::BMW_WIDTH_SPRITE);
            break;
        case Constants::JEEP:
            car = std::make_shared<Car>(world, position, carType, Constants::JEEP_BASE_ACCELERATION,
                                        Constants::JEEP_BASE_CONTROL, Constants::JEEP_BASE_WEIGHT,
                                        Constants::JEEP_BASE_MAX_SPEED, Constants::JEEP_BASE_REVERSE_SPEED,
                                        Constants::JEEP_BASE_HEALTH, Constants::JEEP_HEIGHT_SPRITE, Constants::JEEP_WIDTH_SPRITE);
            break;
        case Constants::CIVIC:
            car = std::make_shared<Car>(world, position, carType, Constants::CIVIC_BASE_ACCELERATION,
                                        Constants::CIVIC_BASE_CONTROL, Constants::CIVIC_BASE_WEIGHT,
                                        Constants::CIVIC_BASE_MAX_SPEED, Constants::CIVIC_BASE_REVERSE_SPEED,
                                        Constants::CIVIC_BASE_HEALTH, Constants::CIVIC_HEIGHT_SPRITE, Constants::CIVIC_WIDTH_SPRITE);
            break;
        case Constants::TRUCK:
            car = std::make_shared<Car>(world, position, carType, Constants::TRUCK_BASE_ACCELERATION,
                                        Constants::TRUCK_BASE_CONTROL, Constants::TRUCK_BASE_WEIGHT,
                                        Constants::TRUCK_BASE_MAX_SPEED, Constants::TRUCK_BASE_REVERSE_SPEED,
                                        Constants::TRUCK_BASE_HEALTH, Constants::TRUCK_HEIGHT_SPRITE, Constants::TRUCK_WIDTH_SPRITE);
            break;
        default:
            std::cerr << "Tipo de auto desconocido: " << carType << std::endl;
            return;
    }
    std::cout << "Auto agregado de tipo: " << carType << std::endl;
    cars.push_back(car);
}


void RaceBuilder::addStaticNpcCar(int carType, Vector2D<float> basePosition) {
    std::shared_ptr<Car> car;
    switch (carType) {

        case Constants::FORD:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, Constants::FORD_HEIGHT_SPRITE, Constants::FORD_WIDTH_SPRITE);
            break;
        case Constants::MAZDA:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, Constants::MAZDA_HEIGHT_SPRITE, Constants::MAZDA_WIDTH_SPRITE);
            break;
        case Constants::CORROLLA:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, Constants::CORROLLA_HEIGHT_SPRITE, Constants::CORROLLA_WIDTH_SPRITE);
            break;
        case Constants::BMW:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, Constants::BMW_HEIGHT_SPRITE, Constants::BMW_WIDTH_SPRITE);
            break;
        case Constants::JEEP:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, Constants::JEEP_HEIGHT_SPRITE, Constants::JEEP_WIDTH_SPRITE);
            break;
        case Constants::CIVIC:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, Constants::CIVIC_HEIGHT_SPRITE, Constants::CIVIC_WIDTH_SPRITE);
            break;
        case Constants::TRUCK:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, Constants::TRUCK_HEIGHT_SPRITE, Constants::TRUCK_WIDTH_SPRITE);
            break;    
        default:
            std::cerr << "Tipo de auto desconocido para NPC estático: " << carType << std::endl;
            return;
    }
    std::cout << "Auto NPC estático agregado de tipo: " << carType << std::endl;
    staticNpcs.push_back(car);
}


b2WorldId RaceBuilder::getWorld() const {
    return world;
}


std::vector<std::shared_ptr<Car>>& RaceBuilder::getCars() {
    return cars;
}


std::vector<std::shared_ptr<Car>>& RaceBuilder::getStaticNpcs() {
    return staticNpcs;
}
