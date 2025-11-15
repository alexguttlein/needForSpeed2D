#ifndef MAP_ENTITIES_H
#define MAP_ENTITIES_H

#include "vector2D.h"
#include <vector>
#include <box2d/box2d.h>

/*
* Estructura que representa una entidad del mapa
* con sus posiciónes, tipo y si es colisionable o no.
*
* */
struct MapObject {
    float x;
    float y;
    float width;
    float height;
    bool isPolygon;
    std::vector<b2Vec2> polygonPoints;
};
#endif // MAP_ENTITIES_H
