#ifndef CAR_H
#define CAR_H

#include <cmath>
#include <box2d/box2d.h>
#include "../common/constants.h"
#include "../common/vector2D.h"

class Car {
private:

    // Box2D body
    b2BodyId body; 
    b2WorldId world;

    // posiciones
    Vector2D<float> position;  
    Vector2D<float> direction;
    
    // fisica del auto
    float acceleration, control, weight, speed;
    float maxSpeed, maxReverseSpeed;
    float friction = 2.0f;

    // estado del auto
    float health;
    float maxHealth = health;
    bool destroyed = false;

public:
    /*
    * Constructor de Car.
    * Inicializa los atributos del auto con los valores recibidos por parámetro.
    * */
    explicit Car(b2WorldId world,Vector2D<float> position, float acceleration, float control,
        float weight, float maxSpeed, float maxReverseSpeed, float health);

    /*
    * Configura el cuerpo Box2D del auto
    *
    * */
    void setCarBox2DBody(Vector2D<float> position);

    /*
    * Obtiene el cuerpo Box2D del auto
    *
    * */
    b2BodyId getBody() const;

    /*
    * Obtiene la posicion del auto en el plano
    *
    * */
    Vector2D<float> getPosition() const;
    
    /*
    * Obtiene la dirección del auto como un vector normalizado
    *
    * */
    Vector2D<float> getDirection() const;

    /*
    * Obtiene la velocidad del auto
    *
    * */
    float getSpeed() const;

    /*
    * calcula la velocidad del auto en el sistema Box2D
    *
    * */
    float getBoxSpeed() const;

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
    * Calcula la fuerza a aplicar para acelerar o frenar el auto
    *
    * */
    b2Vec2 getForce(bool accelerate) const;

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
    * Aplica fricción al auto para reducir su velocidad, lo hace segun el peso y el impulso
    *
    * */
    void applyFriction();

    /*
    * Obtiene la velocidad lateral del auto a partir de su cuerpo Box2D
    *
    * */
    b2Vec2 getLateralVelocity() const;

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
