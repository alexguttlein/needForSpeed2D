#ifndef SETTINGS_LOADER_H
#define SETTINGS_LOADER_H

#include "../common/constants.h"
#include <string>
#include <yaml-cpp/yaml.h>
#include <iostream>

class SettingsLoader {

public:

    /*
    * Función estática para cargar y configurar GameSettings
    *
    * */
    static void load(const std::string& filepath);

private:

    /*
    *   Función auxiliar para leer un nodo float con manejo de errores
    *
    * */ 
    static float getFloat(const YAML::Node& node, const std::string& key);

    /*
    * Función auxiliar para leer un nodo int con manejo de errores
    *
    * */
    static int getInt(const YAML::Node& node, const std::string& key);
};
#endif // SETTINGS_LOADER_H
