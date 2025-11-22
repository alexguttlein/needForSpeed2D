#include "mapLoader.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

bool MapLoader::isSafeCoord(float c) {
    // Usamos 1,000,000.0f como límite superior seguro, mucho menor que el b2_huge de Box2D.
    const float MAX_COORD = 1000000.0f;
    return std::isfinite(c) && (c > -MAX_COORD) && (c < MAX_COORD);
}



float MapLoader::extractFloatValue(const std::string& line) {
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




Map MapLoader::loadMapFromYaml(const std::string& filepath) {
    Map map;
    std::ifstream file(filepath);
    std::string line;
    bool parsingCheckpoints = false;
    Vector2D<float> currentCheckpoint = {0.0f, 0.0f};

    std::cout << "[MapLoader] Intentando cargar mapa desde: " << filepath << std::endl;

    if (!file.is_open()) {
        std::cerr << "[MapLoader ERROR] No se pudo abrir el archivo: " << filepath << ". Fallback a valores por defecto." << std::endl;
        // Fallback robusto
        map.width = 800;
        map.height = 600;
        map.checkpoints.push_back({25.0f, 12.0f});
        return map;
    }

    while (std::getline(file, line)) {
        // Eliminar espacios iniciales
        line.erase(0, line.find_first_not_of(" \t"));

        if (line.empty() || line[0] == '#') {
            continue; // Saltar líneas vacías o comentarios
        }

        // --- Búsqueda de Dimensiones ---
        if (line.rfind("ancho:", 0) == 0) {
            map.width = static_cast<int>(extractFloatValue(line));

        } else if (line.rfind("alto:", 0) == 0) {
            map.height = static_cast<int>(extractFloatValue(line));

        // --- Inicio de la Secuencia de Checkpoints ---
        } else if (line.rfind("checkpoints:", 0) == 0) {
            parsingCheckpoints = true;
            continue;

        // --- Parseo de Checkpoints ---
        } else if (parsingCheckpoints) {

            // Si la línea contiene '- x:', es el inicio de un nuevo checkpoint
            if (line.find("- x:") != std::string::npos) {
                // Si encontramos '- x:', significa que la línea contiene el valor de X
                currentCheckpoint.x = extractFloatValue(line);
                currentCheckpoint.y = 0.0f; // Reiniciamos Y

            } else if (line.find("y:") != std::string::npos) {
                // Si la línea contiene 'y:', es el valor de Y
                currentCheckpoint.y = extractFloatValue(line);

                // Si ya tenemos X e Y (X debe ser diferente de 0.0f del reset)
                if (currentCheckpoint.x != 0.0f || currentCheckpoint.y != 0.0f) {
                    map.checkpoints.push_back(currentCheckpoint);
                    // Reset para el próximo checkpoint (importante, ya que 0,0 puede ser una coord válida)
                    currentCheckpoint = {0.0f, 0.0f};
                }
            }
            // Ignoramos otras líneas dentro del bloque 'checkpoints' que no sean '- x:' o 'y:'
        }
    }

    std::cout << "[MapLoader] Dimensiones: " << map.width << "x" << map.height << ". Total checkpoints cargados: " << map.checkpoints.size() << std::endl;
    return map;
}




std::vector<MapObject> MapLoader::loadCollidersFromYaml(const std::string& filepath) {
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