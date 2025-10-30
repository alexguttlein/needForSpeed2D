#ifndef CAR_H
#define CAR_H

#include <cmath>
#include "common/constants.h"
#include "common/vector2D.h"

class Car {
private:
    // posiciones
    Vector2D<float> position;  
    Vector2D<float> direction;
    
    // fisica del auto
    float acceleration, control, weight;
    float speed = Constants::INITIAL_SPEED;
    float maxSpeed, maxReverseSpeed;
    float friction = Constants::FRICTION_BASE;

    // estado del auto
    float health;
    float maxHealth = health;
    bool destroyed = false;

public:
    /*
    * Constructor de Car.
    * Inicializa los atributos del auto con los valores recibidos por parámetro.
    * */
    explicit Car(Vector2D<float> position, float acceleration, float control,
        float weight, float maxSpeed, float maxReverseSpeed, float health);

    /*
    * Obtiene la posicion del auto en el plano
    *
    * */
    Vector2D<float> getPosition() const;
    
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
    * Funcion auxiliar para rotar un vector en un ángulo dado,
    * usada para evitar codigo repetido en turnLeft y turnRight.
    * */
    static Vector2D<float> rotateVec(const Vector2D<float>& v, float angle);

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
    * reduce la salud del auto según el daño recibido
    *
    * */
    void takeDamage(float damage);

    /*
    * Mejora la salud del auto, el min es para que no supere la salud máxima
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
