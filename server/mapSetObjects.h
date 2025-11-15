#ifndef MAP_SET_OBJECTS_H
#define MAP_SET_OBJECTS_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <box2d/box2d.h>
#include "../common/constants.h"
#include "../common/mapEntities.h"


class MapSetObjects {


public:


    /*
    * Crea un cuerpo estático en el mundo Box2D con las formas dadas
    *
    * */
    void createStaticBody(b2WorldId world, const b2Vec2& position, const std::vector<b2Polygon>& fixtures);

    

    void createBodiesFromObjects(b2WorldId world, const std::vector<MapObject>& objects);

};
#endif // MAP_SET_OBJECTS_H
