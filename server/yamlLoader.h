#ifndef MAPLOADER_H
#define MAPLOADER_H

#include "../common/map.h"
#include "../common/vector2D.h"
#include "../common/mapEntities.h"
#include <string>
#include <iostream>
#include <vector>
#include <box2d/box2d.h>
#include <yaml-cpp/yaml.h>
#include "server/raceSpawnData.h"


class YamlLoader {
private:
    
    bool isSafeCoord(float c);
    static float extractFloatValue(const std::string& line);
    
public:
   
    static Map loadMapFromYaml(const std::string& filepath);
    static std::vector<MapObject> loadCollidersFromYaml(const std::string& filepath);
    std::unordered_map<int, RaceSpawnData> loadRaceSpawnPositions(const std::string& filepath);

};

#endif // MAPLOADER_H