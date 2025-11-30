#include "car.h"
#include <iostream>

Car::Car(b2WorldId world,Vector2D<float> position, int carType, float acceleration, float control,
    float weight, float maxSpeed, float maxReverseSpeed, 
    float health, float height, float width)
    : world(world) 
    , position(position)
    , carType(carType)
    , acceleration(acceleration)
    , control(control)
    , weight(weight)
    , maxSpeed(maxSpeed)
    , maxReverseSpeed(maxReverseSpeed)
    , health(health)
    , height(height)
    , width(width)
    , baseMaxHealth(health){

        setCarBox2DBody(position);

    }


void Car::setCarBox2DBody(Vector2D<float> position) {

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {position.x, position.y};
    bodyDef.enableSleep = false; 
    bodyDef.linearDamping = Constants::DEFAULT_LINEAR_DAMPING; // aplica fricción lineal
    bodyDef.angularDamping = Constants::DEFAULT_ANGULAR_DAMPING; // aplica fricción angular
    body = b2CreateBody(world, &bodyDef);
    b2Body_SetAwake(body, true);

    // Forma del auto: rectángulo simple
    b2Polygon shape = b2MakeBox(width, height);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = weight;    
    shapeDef.enableContactEvents = true; 

    // Collision filtering
    shapeDef.filter.categoryBits = 0x0001;
    shapeDef.filter.maskBits = 0x0001;
    shapeDef.userData = this;          

    b2ShapeId shapeId = b2CreatePolygonShape(body, &shapeDef, &shape);
    b2Shape_SetFriction(shapeId, Constants::FRICTION_BASE);
    b2Shape_SetRestitution(shapeId, Constants::DEFAULT_RESTITUTION);
}


b2BodyId Car::getBody() const {
    return body;
}


Vector2D<float> Car::getPosition() const {
    b2Vec2 pos = b2Body_GetPosition(body);
    return Vector2D<float>(pos.x, pos.y);
}


void Car::setPosition(const Vector2D<float>& newPosition) {
    b2Body_SetTransform(body, b2Vec2{newPosition.x, newPosition.y}, b2Body_GetRotation(body));
}


void Car::setRotation(float angleDegrees) {
    const float PI = 3.14159265359f; 
    float angleRadians = angleDegrees * (PI / 180.0f);
    b2Rot angle = b2MakeRot(angleRadians);
    b2Body_SetTransform(body, b2Body_GetPosition(body), angle);
}


void Car::setRotationToRight() {
   setRotation(0.0f);
}


Vector2D<float> Car::getDirection() const {
    b2Rot angle = b2Body_GetRotation(body);
    return Vector2D<float>(angle.c, angle.s);
}


void Car::setIsAccelerating(bool value) {
    isAccelerating = value;
}


void Car::setIsBraking(bool value) {
    isBraking = value;
}


void Car::setIsTurningLeft(bool value) {
    isTurningLeft = value;
}


void Car::setIsTurningRight(bool value) {
    isTurningRight = value;
}


float Car::getSpeed() const {
   b2Vec2 velocity = b2Body_GetLinearVelocity(body);
   return std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
}


float Car::getBoxSpeed() const {
    b2Vec2 velocity = b2Body_GetLinearVelocity(body);
    b2Rot rotation = b2Body_GetRotation(body);
    b2Vec2 forward{rotation.c, rotation.s};
    return velocity.x * forward.x + velocity.y * forward.y;
}


float Car::getHealth() const {
    return health;
}


bool Car::isDestroyed() const {
    return destroyed;
}


b2Vec2 Car::getForce(bool accelerate) const {
    b2Rot dir = b2Body_GetRotation(body);
    b2Vec2 dirBox{dir.c, dir.s};
    float f = accelerate ? acceleration : -acceleration;
    return dirBox * (f * accelerationMultiplier);
}


void Car::accelerate() {
    float speed = getBoxSpeed();
    if (speed >= (maxSpeed * speedMultiplier)) return;
    b2Vec2 force = getForce(true);
    b2Body_ApplyForceToCenter(body, force, true);
}


void Car::breakReverse() {
    float speed = getBoxSpeed();
    if (speed <= -maxReverseSpeed) return;
    b2Vec2 force = getForce(false);
    b2Body_ApplyForceToCenter(body, force, true);
}


void Car::turnLeft() {
    float speed = getBoxSpeed();
    float directionFactor = (speed < 0.0f) ? -1.0f : 1.0f;
    if (std::abs(speed) > 0.5f) {
        b2Body_ApplyTorque(body, -control * controlMultiplier * directionFactor, true);
    }
}

void Car::turnRight() {
    float speed = getBoxSpeed();
    float directionFactor = (speed < 0.0f) ? -1.0f : 1.0f;
    if (std::abs(speed) > 0.5f) {
        b2Body_ApplyTorque(body, control * controlMultiplier * directionFactor, true);
    }
}


void Car::applyFriction() {
    b2Vec2 lateralVelocity = getLateralVelocity();
    b2Vec2 impulse = lateralVelocity * -b2Body_GetMass(body) * friction;
    b2Body_ApplyLinearImpulse(body, impulse, b2Body_GetPosition(body), true);
}


void Car::applyMovement() {
    if(isDestroyed()) return;
    
    if (isAccelerating) {
        accelerate();
    }
    if (isBraking) {
        breakReverse();
    }
    if (isTurningLeft) {
        turnLeft();
    }
    if (isTurningRight) {
        turnRight();
    }
}


b2Vec2 Car::getLateralVelocity() const {
    b2Vec2 currentVelocity = b2Body_GetLinearVelocity(body);
    b2Rot rotation = b2Body_GetRotation(body);
    
    b2Vec2 rightVec{-rotation.s, rotation.c};
    float lateralSpeed = currentVelocity.x * rightVec.x + currentVelocity.y * rightVec.y;
    return rightVec * lateralSpeed;
}


void Car::takeDamage(float damage) {
    health -= damage;
    if (health <= Constants::NO_HEALTH) {
        health = Constants::NO_HEALTH;
        // detenemos el cuerpo en box2D
        b2Body_SetLinearVelocity(body, {0.0f, 0.0f}); 
        b2Body_SetAngularVelocity(body, 0.0f);
        destroyed = true;
    }
}


void Car::upgradeControl() {
    controlMultiplier *= Constants::CONTROL_UPGRADE_VALUE;
    b2Body_SetAngularDamping(body, b2Body_GetAngularDamping(body) * 0.8f);
}


void Car::upgradeAcceleration() {
   accelerationMultiplier *= Constants::ACCELERATION_UPGRADE_VALUE;
   std:: cout << "Upgraded acceleration. New acceleration multiplier: " << accelerationMultiplier << std::endl;
}


void Car::upgradeHealth(){
    maxHealth += Constants::HEALTH_UPGRADE_VALUE; 
    health = std::min(maxHealth, health + Constants::HEALTH_UPGRADE_VALUE); 
}


void Car::upgradeSpeed() {
    speedMultiplier *= Constants::MAX_SPEED_UPGRADE_VALUE;
    b2Body_SetLinearDamping(body, b2Body_GetLinearDamping(body) * 0.8f);
}


Car::~Car() {
    // b2DestroyBody(body);
}


void Car::destroyBody() {
    if (b2Body_IsValid(body)) {
        b2DestroyBody(body);
        body = b2BodyId{}; 
    }
}


void Car::resetVelocity() {
    b2Body_SetLinearVelocity(body, b2Vec2{0.0f, 0.0f});
    b2Body_SetAngularVelocity(body, 0.0f);
}


void Car::resetMovementStates() {
    isAccelerating = false;
    isBraking = false;
    isTurningLeft = false;
    isTurningRight = false;
}


void Car::applyUpgrade(int upgradeId) {     
    if (upgradeId == Constants::HEALTH_UPGRADE_ID) { 
        upgradeHealth();
    } else if (upgradeId == Constants::ACCELERATION_UPGRADE_ID) { 
        upgradeAcceleration();
    } else if (upgradeId == Constants::CONTROL_UPGRADE_ID) { 
        upgradeControl();
    } else if (upgradeId == Constants::MAX_SPEED_UPGRADE_ID) { 
        upgradeSpeed();
    }
    currentUpgradeId = upgradeId;
}


void Car::clearUpgradeEffects() {
    
    b2Body_SetLinearDamping(body, Constants::DEFAULT_LINEAR_DAMPING);
    b2Body_SetAngularDamping(body, Constants::DEFAULT_ANGULAR_DAMPING);
    accelerationMultiplier = Constants::DEFAULT_MULTIPLIER;
    speedMultiplier = Constants::DEFAULT_MULTIPLIER;
    controlMultiplier = Constants::DEFAULT_MULTIPLIER;

    if (currentUpgradeId == Constants::HEALTH_UPGRADE_ID) {
        maxHealth = baseMaxHealth; 
        health = std::min(health, baseMaxHealth);
    }
    currentUpgradeId = Constants::DEFAULT_UPGRADE_ID;
}


int Car::getCurrentUpgradeId() {
    return currentUpgradeId;
}

int Car::getCarType() const {
    return carType;
}
