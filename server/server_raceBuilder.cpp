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


// por ahora asi, mas adelante se deberia poner a cada auto segun como salio en la carrera
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
            car = std::make_shared<Car>(world, position, carType, 50.0f, 40.0f, 6.0f, 50.0f, 50.0f, 90.0f, 1.20f / 2.0f, 1.28f / 2.0f);
            break;
        case Constants::MAZDA:
            car = std::make_shared<Car>(world, position, carType, 65.0f, 70.0f, 3.0f, 70.0f, 40.0f, 80.0f, 1.55f/2.0f,1.64f/2.0f);
            break;
        case Constants::CORROLLA:
            car = std::make_shared<Car>(world, position, carType, 55.0f, 55.0f, 6.0f, 60.0f, 50.0f, 100.0f, 1.45f/2.0f,1.64f/2.0f);
            break;
        case Constants::BMW:
            car = std::make_shared<Car>(world, position, carType, 60.0f, 70.0f, 3.0f, 50.0f, 60.0f, 70.0f, 1.48f/2.0f,1.68f/2.0f);
            break;
        case Constants::JEEP:
            car = std::make_shared<Car>(world, position, carType, 40.0f, 70.0f, 3.0f, 50.0f, 50.0f, 110.0f, 1.48f/2.0f,1.68f/2.0f);
            break;
        case Constants::CIVIC:
            car = std::make_shared<Car>(world, position, carType, 50.0f, 70.0f, 5.0f, 50.0f, 50.0f, 100.0f, 1.48f/2.0f,1.64f/2.0f);
            break;
        case Constants::TRUCK:
            car = std::make_shared<Car>(world, position, carType, 50.0f, 90.0f, 5.0f, 50.0f, 50.0f, 150.0f, 1.77f/2.0f,2.04f/2.0f);
            break;
        default:
            std::cerr << "Tipo de auto desconocido: " << carType << std::endl;
            return;
    }
    std::cout << "Auto agregado de tipo: " << carType << std::endl;
    cars.push_back(car);
}


// fijarse que aparezcan bien porque creo que esta seteado que aparezcan apanzados
void RaceBuilder::addStaticNpcCar(int carType, Vector2D<float> basePosition) {
    std::shared_ptr<Car> car;
    switch (carType) {

        case Constants::FORD:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, 1.20f / 2.0f, 1.28f / 2.0f);
            break;
        case Constants::MAZDA:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, 1.55f/2.0f,1.64f/2.0f);
            break;
        case Constants::CORROLLA:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, 1.45f/2.0f,1.64f/2.0f);
            break;
        case Constants::BMW:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, 1.48f/2.0f,1.68f/2.0f);
            break;
        case Constants::JEEP:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, 1.48f/2.0f,1.68f/2.0f);
            break;
        case Constants::CIVIC:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, 1.48f/2.0f,1.64f/2.0f);
            break;
        case Constants::TRUCK:
            car =  std::make_shared<Car>(world, basePosition, carType, Constants::STATIC_NPC_ACCELERATION,
                Constants::STATIC_NPC_CONTROL, Constants::STATIC_NPC_WEIGHT,
                Constants::STATIC_NPC_MAX_SPEED, Constants::STATIC_NPC_MAX_REVERSE_SPEED,
                Constants::STATIC_NPC_HEALTH, 1.77f/2.0f,2.04f/2.0f);
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