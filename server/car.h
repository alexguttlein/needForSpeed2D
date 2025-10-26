#ifndef CAR_H
#define CAR_H

#include <cmath>

class Car {

private:
    float x, y;          
    float speed;
    float direction;
    float acceleration;
    float health;

public:

    /*
    * Constructor de la clase Car
    *
    * */
    explicit Car(float start_x, float start_y);
    
    /*
    * Acelera el auto
    *
    *  */
    void accelerate(float distance);

    /*
    * Gira el auto a la derecha
    *
    * */
    void turn_right(float angle);
    

    /*
    * Gira el auto a la izquierda
    * */
    void turn_left(float angle);


    /*
    * Mueve el auto
    *
    * */
    void move(float distance);


    /*
    * Retrocede el auto
    *
    * */
    void reverse(float distance);


    /*
    * Obtiene la posición x del auto
    *
    * */
    float get_x() const;


    /*
    * Obtiene la posición y del auto
    *
    * */
    float get_y() const;


    /*
    * Obtiene la salud del auto
    *
    * */
    float get_health() const;
};
#endif // CAR_H
