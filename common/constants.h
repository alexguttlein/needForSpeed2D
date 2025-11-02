#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace  Constants {

    // constantes salida de programa
    static constexpr int SUCCESS = 0;
    static constexpr int ERROR = -1;

    // constantes de parámetros de línea de comandos
    static constexpr int EXPECTED_CLIENT_ARGC = 3;
    static constexpr int EXPECTED_SERVER_ARGC = 2;
    static constexpr int HOST_CLIENT_ARG_INDEX = 1;
    static constexpr int PORT_CLIENT_ARG_INDEX = 2;
    static constexpr int PORT_SERVER_ARG_INDEX = 1;

    // constantes de tamanios de queues
    static const unsigned int CLIENT_QUEUE_MAXSIZE = 500;

    // constantes de ingresos del cliente
    static const std::string INPUT_EXIT = "exit";

    // constantes de errores
    static const std::string ERROR_PARAMETERS_QUANTITY =
        "Error: La cantidad de parametros no es correcta";
    static const std::string ERROR_EXC_CAPTURADA_CLIENT_MAIN =
        "Error: Excepción capturada en client_main: ";
    static const std::string ERROR_EXC_CAPTURADA_SERVER_MAIN =
        "Error: Excepción capturada en server_main: ";
    static const std::string ERROR_EXC_DESCONOCIDA_CLIENT_MAIN =
        "Error: Excepción desconocida en client_main: ";
    static const std::string ERROR_EXC_DESCONOCIDA_SERVER_MAIN =
        "Error: Excepción desconocida en server_main: ";


    // constante tiempo sleep de loop
    static constexpr int THREAD_SLEEP_MS = 50;


    //constantes del juego
    static constexpr float HEALTH_UPGRADE = 20.0f;
    static constexpr float ACCELERATION_UPGRADE = 5.0f;
    static constexpr float CONTROL_UPGRADE = 0.1f;
    static constexpr float NO_HEALTH = 0.0f;
    static constexpr float FRICTION_BASE = 0.1f;
    static constexpr float INITIAL_SPEED = 0.0f;
}
#endif //CONSTANTS_H
