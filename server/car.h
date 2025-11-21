#ifndef CAR_H
#define CAR_H

#include <cmath>
#include <box2d/box2d.h>
#include "../common/constants.h"
#include "../common/vector2D.h"

class Car {
private:

    // Box2D body
    b2BodyId body{};
    b2WorldId world{};

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
    float speedMultiplier = 1.0f;
    float accelerationMultiplier = 1.0f;
    float controlMultiplier = 1.0f;
    int currentUpgradeId = 0;


    // estados de movimiento
    bool isAccelerating = false;
    bool isBraking = false;
    bool isTurningLeft = false;
    bool isTurningRight = false;

    float height, width;

public:
    /*
    * Constructor de Car.
    * Inicializa los atributos del auto con los valores recibidos por parámetro.
    * */
    explicit Car(b2WorldId world,Vector2D<float> position, float acceleration, float control,
        float weight, float maxSpeed, float maxReverseSpeed, float health, float height, float width);

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
    * Establece la posición del auto en el plano
    *
    * */
    void setPosition(const Vector2D<float>& newPosition);
    
    /*
    * Obtiene la dirección del auto como un vector normalizado
    *
    * */
    Vector2D<float> getDirection() const;

    /*
    * setea si el auto está acelerando 
    *
    * */
    void setIsAccelerating(bool value);

    /*
    * setea si el auto está frenando
    *
    * */
    void setIsBraking(bool value);

    /*
    * setea si el auto está girando a la izquierda
    *
    * */
    void setIsTurningLeft(bool value);

    /*
    * setea si el auto está girando a la derecha
    *
    * */
    void setIsTurningRight(bool value);

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
    * Aplica el movimiento al auto según su estado actual
    *
    * */
    void applyMovement();

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
    
    /*
    * Mejora la velocidad máxima del auto
    *
    * */
    void upgradeSpeed();
    
    /*
    * Destructor de Car
    *
    * */
    ~Car();
    
    /*
    * Destruye el cuerpo Box2D del auto de manera segura
    *
    * */
    void destroyBody(); 

    /*
    * Obtiene el ID del cuerpo Box2D del auto
    *
    * */    
    b2BodyId getBodyId() const { return body; }

    /*
    * Resetea la velocidad del auto a cero
    *
    * */
    void resetVelocity();    
    
    /*
    * Resetea los estados de movimiento del auto
    *
    * */
    void resetMovementStates();
    
    /*
    * Aplica una mejora al auto según el ID de la mejora
    *
    * */
    void applyUpgrade(int upgradeId);
    
    /*
    * Limpia los efectos de las mejoras aplicadas al auto
    *
    * */
    void clearUpgradeEffects();
};
#endif // CAR_H
