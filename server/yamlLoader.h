#ifndef MAPLOADER_H
#define MAPLOADER_H

#include "../common/map.h"
#include "../common/vector2D.h"
#include "../common/mapEntities.h"
#include "npcData.h"

#include <string>
#include <iostream>
#include <vector>
#include <box2d/box2d.h>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>


class YamlLoader {
private:
    
    /*
    * Verifica si una coordenada es segura (no NaN ni infinita).
    *
    * */
    bool isSafeCoord(float c);

    /*
    * Extrae un valor float de una línea con formato 'key: value'.
    *
    * */
    static float extractFloatValue(const std::string& line);
    
public:
    
    /*
    * Carga un mapa desde un archivo YAML y devuelve un objeto Map.
    *
    * */
    static Map loadMapFromYaml(const std::string& filepath);

    /*
    * Carga colisionadores desde un archivo YAML y devuelve un vector de MapObject.
    *
    * */
    static std::vector<MapObject> loadCollidersFromYaml(const std::string& filepath);

    /*
    * Carga NPCs desde un archivo YAML y devuelve un vector de NPCData.
    *
    * */
    static std::vector<NPCData> loadNPCsFromYaml(const std::string& filepath);
};
#endif // MAPLOADER_H
