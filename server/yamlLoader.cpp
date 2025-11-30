#include "yamlLoader.h"

float YamlLoader::extractFloatValue(const std::string& line) {
    try {
        size_t pos = line.find(':');
        if (pos == std::string::npos)
            return 0.0f;

        std::string value = line.substr(pos + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(remove(value.begin(), value.end(), '"'), value.end());

        if (!value.empty() && (value.back() == ',' || value.back() == '\r'))
            value.pop_back();

        return std::stof(value);
    }
    catch (...) {
        std::cerr << "[MapLoader ERROR] Error al convertir el valor '" << line << "' a float. (stof)\n";
        return 0.0f;
    }
}


Map YamlLoader::loadMapFromYaml(const std::string& filepath) {
    Map map;
    std::cout << "[MapLoader] Intentando cargar TODOS los circuitos desde: " << filepath << std::endl;
    
    try {
        YAML::Node config = YAML::LoadFile(filepath);

        if (config["mapa"]) {
            map.width  = config["mapa"]["ancho"].as<int>(800);
            map.height = config["mapa"]["alto"].as<int>(600);
        } else {
            std::cerr << "[MapLoader WARNING] No se encontró la sección 'mapa'. Usando valores por defecto." << std::endl;
        }

        YAML::Node circuitsNode = config["circuitos"];
        if (!circuitsNode || !circuitsNode.IsMap()) {
            throw std::runtime_error("No se encontró o 'circuitos' no es un mapa válido.");
        }
        
        for (YAML::const_iterator it = circuitsNode.begin(); it != circuitsNode.end(); ++it) {
            std::string raceId = it->first.as<std::string>(); 
            YAML::Node raceNode = it->second;
            
            RaceCircuit currentCircuit;
            currentCircuit.race_id = raceId;
 
            if (raceNode["checkpoints"] && raceNode["checkpoints"].IsSequence()) {
                for (const auto& checkpointNode : raceNode["checkpoints"]) {
                    float x = checkpointNode["x"].as<float>(0.0f);
                    float y = checkpointNode["y"].as<float>(0.0f);
                    currentCircuit.checkpoints.push_back({x, y});
                }
            }

            if (currentCircuit.checkpoints.size() > 0) {
                map.circuits[raceId] = currentCircuit;
            } else {
                 std::cerr << "[MapLoader WARNING] Carrera '" << raceId << "' no tiene puntos válidos y fue omitida." << std::endl;
            }
        }
        
        std::cout << "[MapLoader] Carga completada. Total de circuitos listos: " << map.circuits.size() << std::endl;

    } catch (const YAML::BadFile& e) {
        std::cerr << "[MapLoader FATAL] No se pudo abrir o leer el archivo YAML: " << filepath << std::endl;
        // Fallback simple
    } catch (const std::exception& e) {
        std::cerr << "[MapLoader FATAL] Error al parsear el YAML: " << e.what() << std::endl;
        // Fallback simple
    }
    return map;
}


std::vector<MapObject> YamlLoader::loadCollidersFromYaml(const std::string& filepath) {
    std::vector<MapObject> objects;

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[MapLoader ERROR] No se pudo abrir: " << filepath << std::endl;
        return objects;
    }

    std::string line;
    MapObject current{};
    bool parsingObject = false;
    bool parsingPolygon = false;

    bool readingPointX = false;
    float lastReadX = 0.0f;


    while (std::getline(file, line)) {
    
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line.find('-') == 0) {
            if (line.find(" height:") != std::string::npos || line.find(" id:") != std::string::npos) {

                if (parsingObject) {
                    objects.push_back(current);
                }

                current = {};
                current.isPolygon = false;
                parsingObject = true;
                parsingPolygon = false;
                readingPointX = false;

                if (line.find(" height:") != std::string::npos) {
                    current.height = extractFloatValue(line);
                }
                continue;
            }
        }

        if (!parsingObject) {
            continue;
        }

        if (parsingPolygon) {

            if (line.rfind("- x:", 0) == 0) {
                lastReadX = extractFloatValue(line);
                readingPointX = true;
                continue;

            } else if (readingPointX && line.rfind("y:", 0) == 0) {
                b2Vec2 point;
                point.x = lastReadX / Constants::SCALE_METER_TO_PIXEL;
                point.y = extractFloatValue(line) / Constants::SCALE_METER_TO_PIXEL;

                current.polygonPoints.push_back(point);

                readingPointX = false;
                continue;

            } else if (line.find("rotation:") != std::string::npos || line.find("x:") != std::string::npos ||
                       line.find("width:") != std::string::npos || line.find("height:") != std::string::npos)
            {
                parsingPolygon = false;
                readingPointX = false;
            } else {
                continue;
            }
        }

        if (line.rfind("x:", 0) == 0) {
            current.x = extractFloatValue(line) / Constants::SCALE_METER_TO_PIXEL;
        }
        else if (line.rfind("y:", 0) == 0) {
            current.y = extractFloatValue(line) / Constants::SCALE_METER_TO_PIXEL;
        }
        else if (line.rfind("width:", 0) == 0) {
            current.width = extractFloatValue(line) / Constants::SCALE_METER_TO_PIXEL ;
        }
        else if (line.rfind("height:", 0) == 0) {
            current.height = extractFloatValue(line) / Constants::SCALE_METER_TO_PIXEL ;
        }
        else if (line.rfind("polygon:", 0) == 0) {
            current.isPolygon = true;
            parsingPolygon = true;
        }
    }

    if (parsingObject)
        objects.push_back(current);


    size_t initialCount = objects.size();
    std::vector<MapObject> validObjects;
    int removedCount = 0;

    for (const auto& obj : objects) {
        if (obj.isPolygon || (obj.width > 0.0f && obj.height > 0.0f)) {
            validObjects.push_back(obj);
        } else {
            removedCount++;
        }
    }

    if (removedCount > 0) {
        std::cout << "[MapLoader WARNING] Se descartaron " << removedCount
                  << " objetos con ancho o alto menor/igual a cero (para evitar ASSERT de Box2D)." << std::endl;
    }

    std::cout << "[MapLoader] Total de objetos cargados y válidos: " << validObjects.size() << " (Iniciales: " << initialCount << ")" << std::endl;
    return validObjects;
}


std::vector<NPCData> YamlLoader::loadNPCsFromYaml(const std::string& filepath) {
    std::vector<NPCData> npcs;
    std::cout << "[NPCLoader] Intentando cargar NPCs desde: " << filepath << std::endl;

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[NPCLoader ERROR] No se pudo abrir: " << filepath << std::endl;
        return npcs;
    }

    std::string line;
    NPCData current{};
    bool parsingNPC = false;
    bool readingCoordX = false;
    float lastReadX = 0.0f;

    while (std::getline(file, line)) {

        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line.find("- id:") == 0) {
            
            if (parsingNPC) {
                if (current.id != 0) {
                    npcs.push_back(current);
                } else {
                    std::cerr << "[NPCLoader WARNING] NPC descartado por ID inválido (0): Se requiere un ID único mayor a 0." << std::endl;
                }
            }

            current = {};
            current.id = static_cast<int>(extractFloatValue(line));
            parsingNPC = true;
            readingCoordX = false;
            continue;
        }

        if (!parsingNPC) {
            continue;
        }

        if (line.rfind("carType:", 0) == 0) {
            current.carType = static_cast<int>(extractFloatValue(line));
        } 
        else if (line.rfind("position:", 0) == 0) {
            continue;
        }
        else if (line.rfind("x:", 0) == 0) {
            lastReadX = extractFloatValue(line);
            readingCoordX = true;
        }
        else if (readingCoordX && line.rfind("y:", 0) == 0) {
            current.x = lastReadX;
            current.y = extractFloatValue(line);
            readingCoordX = false;
        }
    }

    if (parsingNPC && current.id != 0) {
        npcs.push_back(current);
    } else if (parsingNPC) {
        std::cerr << "[NPCLoader WARNING] Último NPC descartado por ID inválido (0): Se requiere un ID único mayor a 0." << std::endl;
    }
    std::cout << "[NPCLoader] Total de NPCs cargados y válidos: " << npcs.size() << std::endl;
    return npcs;
}


std::unordered_map<int, RaceSpawnData> YamlLoader::loadRaceSpawnPositions(const std::string& filepath) {

    std::unordered_map<int, RaceSpawnData> result;
    YAML::Node root;

    try {
        root = YAML::LoadFile(filepath);
    } catch (const std::exception& e) {
        std::cerr << "[YamlLoader] Error cargando raceSpawnPositions: "
                  << e.what() << std::endl;
        return result;
    }

    if (!root["spawns"] || !root["spawns"].IsMap()) {
        std::cerr << "[YamlLoader] archivo inválido: falta 'spawns' o no es un mapa."
                  << std::endl;
        return result;
    }

    const YAML::Node& spawnsNode = root["spawns"];

    for (auto it = spawnsNode.begin(); it != spawnsNode.end(); ++it) {
        std::string raceKey = it->first.as<std::string>();
        const YAML::Node& raceNode = it->second;
        int raceId = 0;

        try {
            raceId = std::stoi(raceKey.substr(5)); 
        } catch (...) {
            std::cerr << "[YamlLoader] Clave inválida en spawns: " << raceKey << std::endl;
            continue;
        }

        if (!raceNode["positions"] || !raceNode["positions"].IsSequence()) {
            std::cerr << "[YamlLoader] 'positions' inválido para " << raceKey << std::endl;
            continue;
        }

        RaceSpawnData data;

        for (const auto& posNode : raceNode["positions"]) {
            if (!posNode["x"] || !posNode["y"]) {
                std::cerr << "[YamlLoader] posición inválida en " << raceKey << std::endl;
                continue;
            }

            float x = posNode["x"].as<float>();
            float y = posNode["y"].as<float>();

            data.positions.emplace_back(x, y);
        }

        if (data.positions.empty()) {
            std::cerr << "[YamlLoader] No se cargaron posiciones para " << raceKey << std::endl;
            continue;
        }

        result[raceId] = data;

        std::cout << "[YamlLoader] Cargadas " << data.positions.size()
                  << " posiciones para raceId=" << raceId << std::endl;
    }

    return result;
}

