#include "car.h"


Car::Car(float start_x, float start_y) : x(start_x), y(start_y), 
                                         speed(0.0f), direction(0.0f), acceleration(0.1f), health(100.0f) {}


void Car::accelerate(float distance) {
    acceleration += distance;
}

void Car::turn_right(float angle) {
    direction += angle;
}

void Car::turn_left(float angle) {
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

float Car::get_x() const {
    return x;
}

float Car::get_y() const {
    return y;
}

float Car::get_health() const {
    return health;
}
