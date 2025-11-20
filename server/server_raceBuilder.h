#include <box2d/box2d.h>
#include "../common/vector2D.h"
#include "../common/constants.h"
#include <vector>
#include <memory>
#include "car.h"
#include <iostream>


class RaceBuilder {
private:
    b2WorldId world;
    std::vector<std::shared_ptr<Car>> cars;
    float nextSpawnX;
    float nextSpawnY;
    
public:

    /*
    * Constructor de RaceBuilder
    *
    * */
    RaceBuilder();

    /*
    * Destructor de RaceBuilder
    *
    * */
    ~RaceBuilder();

    /*
    * Devuelve el mundo Box2D
    *
    * */
    b2WorldId getWorld() const;

    /*
    * Agrega todos los autos al mundo de las fisicas y lo devuelve
    *
    * */
    std::vector<std::shared_ptr<Car>>& getCars();

    /*
    * Retorna la posicion de spawn y actualiza la posición para el próximo auto
    * 
    * */
    Vector2D<float> getSpawnPosition();

    /*
    * Setea el punto base de spawn para los autos
    *
    * */
    void setBaseSpawnPoint(const Vector2D<float>& basePoint);

    /*
    * Agrega un auto seleccionado al mundo Box2D
    *
    * */
    void addSelectCar(int carType);
};
