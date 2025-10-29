#include "car.h"

Car::Car(float x, float y, float acceleration, float direction,
    float control, float weight, float maxSpeed, float maxReverseSpeed, 
    float health, float maxHealth)
    : x(x)
    , y(y)
    , acceleration(acceleration)
    , direction(direction)
    , control(control)
    , weight(weight)
    , maxSpeed(maxSpeed)
    , maxReverseSpeed(maxReverseSpeed)
    , health(health)
    , maxHealth(maxHealth) {}

float Car::getX() const {
    return x;
}


float Car::getY() const {
    return y;
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


void Car::turnLeft() {
    float speedFactor = std::max(0.1f, 1.0f - std::abs(speed)/maxSpeed); 
    direction -= control * speedFactor;
}


void Car::turnRight() {
    float speedFactor = std::max(0.1f, 1.0f - std::abs(speed)/maxSpeed); 
    direction += control * speedFactor;
}


void Car::applyFriction() {
    if (speed > 0) {
        speed -= friction;
        if (speed < 0) speed = 0;
    } else if (speed < 0) {
        speed += friction;
        if (speed > 0) speed = 0;
    }
}


void Car::updatePosition() {
    x += speed * std::cos(direction);
    y += speed * std::sin(direction);
}


void Car::takeDamage() {
    health -= 10.0f; // daño fijo por ahora
    if (health <= 0) {
        health = Constants::NO_HEALTH;
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
    health += Constants::HEALTH_UPGRADE;
    if (health > maxHealth) {
        health = maxHealth;
    }
}


void Car::repair() {
    health = maxHealth;
}