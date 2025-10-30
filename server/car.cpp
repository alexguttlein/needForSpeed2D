#include "car.h"

Car::Car(Vector2D<float> position, float acceleration, float control,
    float weight, float maxSpeed, float maxReverseSpeed, 
    float health, float maxHealth)
    : position(position)
    , acceleration(acceleration)
    , control(control)
    , weight(weight)
    , maxSpeed(maxSpeed)
    , maxReverseSpeed(maxReverseSpeed)
    , health(health)
    , maxHealth(maxHealth) {
        direction = Vector2D<float>(1.0f, 0.0f).normalized(); // por ahora apunta a la derecha
    }


Vector2D<float> Car::getPosition() const {
    return position;
}


float Car::getSpeed() const {
    return speed;
}


float Car::getHealth() const {
    return health;
}


bool Car::isDestroyed() const {
    return destroyed;
}


void Car::accelerate() {
    speed += acceleration;
    if (speed > maxSpeed) speed = maxSpeed;
}

void Car::breakReverse() {
    speed -= acceleration;
    if (speed < -maxReverseSpeed) speed = -maxReverseSpeed;
}


static Vector2D<float> rotateVec(const Vector2D<float>& v, float angle) {
    float cosA = std::cos(angle);
    float sinA = std::sin(angle);
    return Vector2D<float>(v.x * cosA - v.y * sinA,
                           v.x * sinA + v.y * cosA).normalized();
}

void Car::turnLeft() {
    float denom = (maxSpeed > 0.0f ? maxSpeed : 1.0f);
    float speedFactor = std::max(0.1f, 1.0f - std::abs(speed) / denom);
    float angle = -control * speedFactor;
    direction = rotateVec(direction, angle);
}

void Car::turnRight() {
    float denom = (maxSpeed > 0.0f ? maxSpeed : 1.0f);
    float speedFactor = std::max(0.1f, 1.0f - std::abs(speed) / denom);
    float angle = control * speedFactor; 
    direction = rotateVec(direction, angle);
}


void Car::applyFriction() {
    if (speed > 0) speed = std::max(0.0f, speed - friction);
    else if (speed < 0) speed = std::min(0.0f, speed + friction);
}


void Car::updatePosition() {
    position = direction * speed;
}


void Car::takeDamage(float damage) {
    health -= damage;
    if (health <= Constants::NO_HEALTH) {
        health = Constants::NO_HEALTH;
        speed = 0.0f;
        destroyed = true;
    }
}


void Car::upgradeControl() {
    control += Constants::CONTROL_UPGRADE;
}


void Car::upgradeAcceleration() {
    acceleration += Constants::ACCELERATION_UPGRADE;
}


void Car::upgradeHealth(){
    health = std::min(maxHealth, health + Constants::HEALTH_UPGRADE);
}


void Car::repair() {
    health = maxHealth;
}