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

    // constantes de manejo de partida
    static constexpr unsigned char CREATE_GAME = 0x20;
    static constexpr unsigned char JOIN_GAME = 0x21;
    static constexpr unsigned char JOIN_REJECTED = 0x22;
    static constexpr unsigned char CREATE_JOIN_ACCEPTED = 0x23;
    static constexpr unsigned char LIST_GAMES = 0x24;
    static constexpr unsigned char TYPE_SNAPSHOT = 0x30;
    static constexpr unsigned char TYPE_CONTROL = 0x31;
    static constexpr unsigned char TYPE_GAME_LIST = 0x32;
    static constexpr int MAX_PLAYERS_IN_GAME = 2; // por ahora

    // constantes de tamanios de queues
    static const unsigned int CLIENT_QUEUE_MAXSIZE = 100;
    static const unsigned int GAME_QUEUE_MAXSIZE = 500;

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
    static constexpr int THREAD_SLEEP_MS = 16; // aprox 60 FPS
    static constexpr int TICKS_PER_SECOND = 60;
    static constexpr int MAX_TICKS = 36000; // 10 minutos a 60 ticks por segundo


    //constantes del juego
    static constexpr float HEALTH_UPGRADE = 20.0f;
    static constexpr float ACCELERATION_UPGRADE = 5.0f;
    static constexpr float CONTROL_UPGRADE = 0.1f;
    static constexpr float NO_HEALTH = 0.0f;
    static constexpr float FRICTION_BASE = 0.1f;
    static constexpr float INITIAL_SPEED = 0.0f;

    static constexpr float SPAWN_START_X = 90.0f;
    static constexpr float SPAWN_START_Y = 90.0f;
    static constexpr float SPAWN_OFFSET_X = 50.0f; 
    static constexpr float MAX_ROW_X = 600.0f;

    static constexpr float CHECKPOINT_RADIUS = 5.0f; 
    static constexpr float HINT_SPACING = 3.0f; // ver como cambia esto con el mapa

    // renderizado con fisica en metros
    static constexpr float SCALE_METER_TO_PIXEL = 25.0f;

}
#endif //CONSTANTS_H
