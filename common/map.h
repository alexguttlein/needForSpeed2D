#ifndef MAP_H
#define MAP_H


#include "vector2D.h"
#include "mapEntities.h"
#include <vector>
#include "../server/car.h"

/*
* Estructura que representa un mapa con sus entidades y puntos clave
* Contiene el ancho y alto del mapa, una lista de entidades del mapa,
* puntos de control, puntos de spawn para carreras y líneas de finalizacion.
*
* */
struct Map {
    int width;
    int height;
    std::vector<MapEntity> entities;
    //std::vector<Car> cars;
    std::vector<Vector2D<float>> checkpoints;
    std::vector<Vector2D<float>> spawnsLines;
    std::vector<Vector2D<float>> finishLines;

    Map() : width(0), height(0) {} // Constructor por defecto
    Map(int width, int height): width(width), height(height) {}
};
#endif // MAP_H
