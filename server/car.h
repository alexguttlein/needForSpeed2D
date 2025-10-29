#ifndef CAR_H
#define CAR_H

#include <cmath>
#include "common/constants.h"

class Car {
private:
    // posiciones
    float x, y;
    
    // fisica del auto
    float acceleration, direction, control, weight;
    float speed = Constants::INITIAL_SPEED;
    float maxSpeed, maxReverseSpeed;
    float friction = Constants::FRICTION_BASE;

    // estado del auto
    float health;
    float maxHealth;
    bool destroyed = false;

public:
    /*
    * Constructor de Car.
    * Inicializa los atributos del auto con los valores recibidos por parámetro.
    * */
    explicit Car(float x, float y, float acceleration, float direction,
        float control, float weight, float maxSpeed, float maxReverseSpeed, 
        float health, float maxHealth);

    /*
    * Obtiene la posicion x del auto
    *
    * */
    float getX() const;
    
    /*
    * Obtiene la posicion y del auto
    *
    * */
    float getY() const;
    
    /*
    * Obtiene la velocidad del auto
    *
    * */
    float getSpeed() const;

    /*
    * Obtiene la salud del auto
    *
    * */ 
    float getHealth() const;

    /*
    * Verifica si el auto está destruido
    *
    * */
    bool isDestroyed() const;

    /*
    * Acelera el auto (aumenta velocidad segun la aceleracion)
    *
    * */
    void accelerate();

    /*
    * Disminuye la velocidad del auto y hace reversa 
    *
    * */
    void breakReverse();

    /*
    * Cambiar la dirección del auto hacia la izquierda, el ángulo depende de la velocidad actual  
    * para ser más realista.
    * */
    void turnLeft();

    /*
    * Cambiar la dirección del auto hacia la derecha, el ángulo depende de la velocidad actual
    * para ser más realista.
    * */
    void turnRight();

    /*
    * Aplica fricción al auto (disminuye la velocidad gradualmente) cuando no se toca ninguna tecla
    *
    * */
    void applyFriction();

    /*
    * Actualiza la posición según la velocidad y dirección, si no toca ninguna tecla
    * empieza a aplicar fricción para reducir la velocidad.
    * */
    void updatePosition();

    /*
    * reduce la salud del auto (por ahora que sea fija)
    *
    * */
    void takeDamage();

    /*
    * Mejora la salud del auto
    *
    * */
    void upgradeHealth();

    /*
    * Mejora la aceleración del auto
    *
    * */
    void upgradeAcceleration();

    /*
    * Mejora el control del auto
    *
    * */
    void upgradeControl();

    /*
    * Repara el auto (restaura la salud al máximo)
    *
    * */
    void repair();
};
#endif // CAR_H
