#ifndef MAP_ENTITIES_H
#define MAP_ENTITIES_H

#include "vector2D.h"
#include <vector>


/*
* Enumeración de tipos de entidades del mapa.
*
* */
enum class MapEntityType {
    GRASS,
    WALL,
    HUMAN_NPC,
    WATER,
    ROAD
};

/*
* Estructura que representa una entidad del mapa
* con sus posiciónes, tipo y si es colisionable o no.
*
* */
struct MapEntity {
    std::vector<Vector2D<float>> positions; // una entidad puede tener múltiples posiciones
    MapEntityType type;
    bool collidable;
};
#endif // MAP_ENTITIES_H
