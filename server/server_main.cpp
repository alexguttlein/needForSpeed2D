#include "server_class.h"
#include "../common/constants.h"

#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {

    try {
        if (argc < Constants::EXPECTED_SERVER_ARGC) {
            std::cerr << Constants::ERROR_PARAMETERS_QUANTITY << std::endl;
            return Constants::ERROR;
        }

        const char* port = argv[Constants::PORT_SERVER_ARG_INDEX];

        Server server(port);
        server.run();

    } catch (const std::exception& e) {
        std::cerr << Constants::ERROR_EXC_CAPTURADA_SERVER_MAIN << e.what() << std::endl;
        return Constants::ERROR;
    } catch (...) {
        std::cerr << Constants::ERROR_EXC_DESCONOCIDA_SERVER_MAIN << std::endl;
        return Constants::ERROR;
    }

    return Constants::SUCCESS;
}