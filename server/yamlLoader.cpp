#include "yamlLoader.h"


bool YamlLoader::isSafeCoord(float c) {
    // Usamos 1,000,000.0f como límite superior seguro, mucho menor que el b2_huge de Box2D.
    const float MAX_COORD = 1000000.0f;
    return std::isfinite(c) && (c > -MAX_COORD) && (c < MAX_COORD);
}


float YamlLoader::extractFloatValue(const std::string& line) {
    try {
        size_t pos = line.find(':');
        if (pos == std::string::npos)
            return 0.0f;

        // agarrar después de los dos puntos
        std::string value = line.substr(pos + 1);

        // limpiar espacios
        value.erase(0, value.find_first_not_of(" \t"));

        // remover comillas
        value.erase(remove(value.begin(), value.end(), '"'), value.end());

        // remover posibles comas finales
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
        // 1. Cargar el archivo YAML completo
        YAML::Node config = YAML::LoadFile(filepath);

        // 2. Extraer dimensiones del Mapa (Nivel 'mapa')
        if (config["mapa"]) {
            map.width  = config["mapa"]["ancho"].as<int>(800);
            map.height = config["mapa"]["alto"].as<int>(600);
        } else {
            std::cerr << "[MapLoader WARNING] No se encontró la sección 'mapa'. Usando valores por defecto." << std::endl;
        }

        // 3. Iterar sobre la sección 'circuitos'
        YAML::Node circuitsNode = config["circuitos"];
        if (!circuitsNode || !circuitsNode.IsMap()) {
            throw std::runtime_error("No se encontró o 'circuitos' no es un mapa válido.");
        }
        
        // Iterar sobre cada circuito (race_1, race_2, etc.)
        for (YAML::const_iterator it = circuitsNode.begin(); it != circuitsNode.end(); ++it) {
            std::string raceId = it->first.as<std::string>(); // Ej: "race_1"
            YAML::Node raceNode = it->second;
            
            RaceCircuit currentCircuit;
            currentCircuit.race_id = raceId;

            // --- 4. Cargar el punto de SPAWN ---
            if (raceNode["Spawn"] && raceNode["Spawn"].IsSequence() && raceNode["Spawn"].size() > 0) {
                YAML::Node spawnCoord = raceNode["Spawn"][0];
                float x = spawnCoord["x"].as<float>(0.0f);
                float y = spawnCoord["y"].as<float>(0.0f);
                
                
                currentCircuit.checkpoints.push_back({x, y}); // Primer punto es el Spawn
               
                    //std::cerr << "[MapLoader WARNING] Spawn de '" << raceId << "' no válido. Ignorando." << std::endl;
                
            } else {
                std::cerr << "[MapLoader WARNING] Carrera '" << raceId << "' no tiene un punto de Spawn definido." << std::endl;
            }
            
            // --- 5. Cargar Checkpoints ---
            if (raceNode["checkpoints"] && raceNode["checkpoints"].IsSequence()) {
                for (const auto& checkpointNode : raceNode["checkpoints"]) {
                    float x = checkpointNode["x"].as<float>(0.0f);
                    float y = checkpointNode["y"].as<float>(0.0f);

                   
                    currentCircuit.checkpoints.push_back({x, y});
                    //std::cerr << "[MapLoader WARNING] Checkpoint en '" << raceId << "' no válido. Ignorando." << std::endl;
                    
                }
            }

            // 6. Almacenar el circuito
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
        // limpiar indentación
        line.erase(0, line.find_first_not_of(" \t"));

        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Detección del inicio de un NUEVO OBJETO con el marcador de lista YAML.
        if (line.find('-') == 0) {
            // Un colisionador comienza con '- height:' o '- id:', no con '- x:'
            if (line.find(" height:") != std::string::npos || line.find(" id:") != std::string::npos) {

                if (parsingObject) {
                    objects.push_back(current);
                }

                // Iniciar un nuevo objeto
                current = {};
                current.isPolygon = false;
                parsingObject = true;
                parsingPolygon = false;
                readingPointX = false;

                // Procesar 'height' si está en la línea de inicio
                if (line.find(" height:") != std::string::npos) {
                    current.height = extractFloatValue(line);
                }
                continue;
            }
        }


        if (!parsingObject) {
            continue;
        }

        // --- 1. PARSING DE PUNTOS DEL POLÍGONO ---
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
                // El bloque del polígono terminó, la línea actual es una propiedad de objeto.
                parsingPolygon = false;
                readingPointX = false;
            } else {
                continue;
            }
        }


        // --- 2. DETECCIÓN DE PROPIEDADES BÁSICAS ---
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

    // Procesa el ÚLTIMO objeto al salir del bucle.
    if (parsingObject)
        objects.push_back(current);

    // -------------------------------------------------------------------------
    // FILTRO DE VALIDACIÓN: Remueve objetos no-poligonales con dimensiones no positivas
    // -------------------------------------------------------------------------

    size_t initialCount = objects.size();
    std::vector<MapObject> validObjects;
    int removedCount = 0;

    for (const auto& obj : objects) {
        // Un objeto es válido si es un polígono (Box2D lo construye a partir de los puntos)
        // O si es un rectángulo y tiene ancho y alto > 0.
        if (obj.isPolygon || (obj.width > 0.0f && obj.height > 0.0f)) {
            validObjects.push_back(obj);
        } else {
            removedCount++;
            // Opcional: imprimir el objeto que se está eliminando para depuración
            // std::cerr << "[MapLoader WARNING] Objeto descartado (dimensión inválida): X=" << obj.x << ", Y=" << obj.y << ", W=" << obj.width << ", H=" << obj.height << std::endl;
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
        // else if (line.rfind("angle:", 0) == 0) {
        //     current.angle = extractFloatValue(line);
        // }
        // --- PARSING DE POSICIÓN (x y) DENTRO DEL BLOQUE 'position:' ---
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
