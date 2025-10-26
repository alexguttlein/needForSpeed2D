#include "car.h"


Car::Car(float start_x, float start_y) : x(start_x), y(start_y), 
                                         speed(0.0f), direction(0.0f), acceleration(0.1f), health(100.0f) {}


void Car::accelerate(float distance) {
    acceleration += distance;
}

void Car::turnRight(float angle) {
    direction += angle;
}

void Car::turnLeft(float angle) {
    direction -= angle;
}


void Car::move(float distance) {
    speed += acceleration * distance;
    x += speed * distance * std::cos(direction);
    y += speed * distance * std::sin(direction);
    acceleration = 0.0f; 
}


void Car::reverse(float distance) {
    x -= speed * distance * std::cos(direction);
    y -= speed * distance * std::sin(direction);
}

float Car::getX() const {
    return x;
}

float Car::getY() const {
    return y;
}

float Car::getHealth() const {
    return health;
}
