#include <fstream>
#include <iostream>

#include "../server/settings_loader.h"
#include "../common/constants.h"
#include "client_class.h"


int main(int argc, char* argv[]) {

    try {
        SettingsLoader::load("settings/game_settings.yaml"); 
    } catch (const std::exception& e) {
        std::cerr << "Error FATAL al cargar la configuración: " << e.what() << std::endl;
        return Constants::ERROR;
    }

    try {
        if (argc < Constants::EXPECTED_CLIENT_ARGC) {
            std::cerr << Constants::ERROR_PARAMETERS_QUANTITY << std::endl;
            return Constants::ERROR;
        }

        const char* host = argv[Constants::HOST_CLIENT_ARG_INDEX];
        const char* port = argv[Constants::PORT_CLIENT_ARG_INDEX];

        Client client(host, port);
        client.run();

    } catch (const std::exception& e) {
        std::cerr << Constants::ERROR_EXC_CAPTURADA_CLIENT_MAIN << e.what() << std::endl;
        return Constants::ERROR;
    } catch (...) {
        std::cerr << Constants::ERROR_EXC_DESCONOCIDA_CLIENT_MAIN << std::endl;
        return Constants::ERROR;
    }

    return Constants::SUCCESS;
}
