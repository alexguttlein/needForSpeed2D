#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>


namespace Constants {

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
    static constexpr unsigned char GAME_START = 0x25;
    static constexpr unsigned char SERVER_DISCONNECTED = 0x26;
    static constexpr unsigned char PLAYER_COUNT_UPDATE = 0x27;
    static constexpr unsigned char TYPE_SNAPSHOT = 0x30;
    static constexpr unsigned char TYPE_CONTROL = 0x31;
    static constexpr unsigned char TYPE_GAME_LIST = 0x32;
   
    // constantes de tamanios de queues
    static const unsigned int CLIENT_QUEUE_MAXSIZE = 100;
    static const unsigned int GAME_QUEUE_MAXSIZE = 500;

    // constantes de ingresos del cliente
    static const std::string INPUT_EXIT = "exit";
    static const std::string INPUT_LISTAR = "listar";
    static const std::string INPUT_CREAR = "crear";
    static const std::string INPUT_UNIRSE = "unirse";
    static const std::string INPUT_START_GAME = "iniciar";

    //constantes menu Qt
    static const std::string ENTER_DRIVER_NAME = "Enter your driver name";
    static const std::string NFS_TITLE = "Need For Speed 2D";
    static const std::string START_BUTTON = "START ENGINE";

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
    static const std::string ERROR_TXT = "Error";
    static const std::string INFO_TXT = "Info";
    static const std::string NO_HAY_PARTIDAS = "No hay partidas activas.";
    static const std::string NO_PUDO_CREAR_PARTIDA = "No se pudo crear la partida.";
    static const std::string NO_ENVIO_SOLICITUD = "No se pudo enviar la solicitud al servidor.";
    static const std::string NO_ENVIO_SOLICITUD_UNIRSE = "No se pudo enviar la solicitud de unirse.";
    static const std::string NO_PUDO_UNIR_PARTIDA = "No se pudo unir a la partida.";
    static const std::string SERVER_ERROR = "No se encuentra el servidor. La aplicación se va a cerrar";
    static const std::string START_GAME_ERROR = "No se pudo enviar START GAME al servidor.";


    static constexpr float SPAWN_START_X = 90.0f;
    static constexpr float SPAWN_START_Y = 90.0f;
    static constexpr float SPAWN_OFFSET_X = 50.0f;
    static constexpr float MAX_ROW_X = 600.0f;
    static constexpr float CHECKPOINT_RADIUS = 2.0f;
    static constexpr float HINT_SPACING = 3.0f;
    static constexpr float SCALE_METER_TO_PIXEL = 25.0f;

    // nombre de movimientos
    static constexpr const char* MOVE_FORDWARD = "w";
    static constexpr const char* MOVE_BACKWARD = "s";
    static constexpr const char* TURN_LEFT = "a";
    static constexpr const char* TURN_RIGHT = "d";

    // constantes representacion autos:
    static constexpr int FORD = 1;
    static constexpr int MAZDA = 2;
    static constexpr int CORROLLA = 3;
    static constexpr int BMW = 4;
    static constexpr int JEEP = 5;
    static constexpr int CIVIC = 6;
    static constexpr int TRUCK = 7;

    inline float FORD_BASE_ACCELERATION;
    inline float MAZDA_BASE_ACCELERATION;
    inline float CORROLLA_BASE_ACCELERATION;
    inline float BMW_BASE_ACCELERATION;
    inline float JEEP_BASE_ACCELERATION;
    inline float CIVIC_BASE_ACCELERATION;
    inline float TRUCK_BASE_ACCELERATION;

    inline float FORD_BASE_MAX_SPEED;
    inline float MAZDA_BASE_MAX_SPEED;
    inline float CORROLLA_BASE_MAX_SPEED;
    inline float BMW_BASE_MAX_SPEED;
    inline float JEEP_BASE_MAX_SPEED;
    inline float CIVIC_BASE_MAX_SPEED;
    inline float TRUCK_BASE_MAX_SPEED;

    inline float FORD_BASE_CONTROL;
    inline float MAZDA_BASE_CONTROL;
    inline float CORROLLA_BASE_CONTROL;
    inline float BMW_BASE_CONTROL;
    inline float JEEP_BASE_CONTROL;
    inline float CIVIC_BASE_CONTROL;
    inline float TRUCK_BASE_CONTROL;

    inline float FORD_BASE_HEALTH;
    inline float MAZDA_BASE_HEALTH;
    inline float CORROLLA_BASE_HEALTH;
    inline float BMW_BASE_HEALTH;
    inline float JEEP_BASE_HEALTH;
    inline float CIVIC_BASE_HEALTH;
    inline float TRUCK_BASE_HEALTH;

    inline float FORD_BASE_WEIGHT;
    inline float MAZDA_BASE_WEIGHT;
    inline float CORROLLA_BASE_WEIGHT;
    inline float BMW_BASE_WEIGHT;
    inline float JEEP_BASE_WEIGHT;
    inline float CIVIC_BASE_WEIGHT;
    inline float TRUCK_BASE_WEIGHT;

    inline float FORD_BASE_REVERSE_SPEED;
    inline float MAZDA_BASE_REVERSE_SPEED;
    inline float CORROLLA_BASE_REVERSE_SPEED;
    inline float BMW_BASE_REVERSE_SPEED;
    inline float JEEP_BASE_REVERSE_SPEED;
    inline float CIVIC_BASE_REVERSE_SPEED;
    inline float TRUCK_BASE_REVERSE_SPEED;

    static constexpr float FORD_WIDTH_SPRITE = 1.28f / 2.0f;
    static constexpr float FORD_HEIGHT_SPRITE = 1.20f / 2.0f;

    static constexpr float MAZDA_WIDTH_SPRITE = 1.64f/2.0f;
    static constexpr float MAZDA_HEIGHT_SPRITE = 1.55f / 2.0f;

    static constexpr float CORROLLA_WIDTH_SPRITE = 1.64f / 2.0f;
    static constexpr float CORROLLA_HEIGHT_SPRITE = 1.45f / 2.0f;

    static constexpr float BMW_WIDTH_SPRITE = 1.68f / 2.0f;
    static constexpr float BMW_HEIGHT_SPRITE = 1.48f / 2.0f;

    static constexpr float JEEP_WIDTH_SPRITE = 1.68f / 2.0f;
    static constexpr float JEEP_HEIGHT_SPRITE = 1.48f / 2.0f;

    static constexpr float CIVIC_WIDTH_SPRITE = 1.64f / 2.0f;
    static constexpr float CIVIC_HEIGHT_SPRITE = 1.48f / 2.0f;

    static constexpr float TRUCK_WIDTH_SPRITE = 2.04f / 2.0f;
    static constexpr float TRUCK_HEIGHT_SPRITE = 1.77f / 2.0f;

    // config juego
    inline int MAX_PLAYERS_IN_GAME; 
    inline int MAX_RACES; 

    // times
    inline int THREAD_SLEEP_MS;
    inline int TICKS_PER_SECOND;
    inline int MAX_RACE_MINUTES;
    
    inline int MAX_TICKS; // Calculado: MAX_RACE_MINUTES * 60 * TICKS_PER_SECOND
    inline float DT;      // Calculado: 1.0f / TICKS_PER_SECOND
    inline int NOT_FINISH_PENALIZE_SECONDS;

    inline int UPGRADE_WAIT_SECONDS;
    inline int UPGRADE_WAIT_TICKS; // Calculado: UPGRADE_WAIT_SECONDS * TICKS_PER_SECOND
    
    // upgrades & penalties

    inline float DEFAULT_PENALIZE;

    inline float HEALTH_UPGRADE_VALUE;
    inline float ACCELERATION_UPGRADE_VALUE;
    inline float CONTROL_UPGRADE_VALUE;
    inline float MAX_SPEED_UPGRADE_VALUE;

    inline float PENALIZE_HEALTH_UPGRADE;
    inline float PENALIZE_CONTROL_UPGRADE;
    inline float PENALIZE_ACCELERATION_UPGRADE;
    inline float PENALIZE_SPEED_UPGRADE;

    // IDs de mejoras
    static constexpr int DEFAULT_UPGRADE_ID = 0;
    static constexpr int HEALTH_UPGRADE_ID = 1;
    static constexpr int ACCELERATION_UPGRADE_ID = 2;
    static constexpr int CONTROL_UPGRADE_ID = 3;
    static constexpr int MAX_SPEED_UPGRADE_ID = 4;

    static const std::string SELECT_HEALTH_UPGRADE = "1";
    static const std::string SELECT_ACCELERATION_UPGRADE = "2";
    static const std::string SELECT_CONTROL_UPGRADE = "3";
    static const std::string SELECT_MAX_SPEED_UPGRADE = "4";


    // física y autos base
    inline float DEFAULT_RESTITUTION;
    inline float DEFAULT_LINEAR_DAMPING;
    inline float DEFAULT_ANGULAR_DAMPING;
    inline float DEFAULT_MULTIPLIER;
    inline float NO_HEALTH;
    inline float FRICTION_BASE;
    inline float INITIAL_SPEED;

    // NPCs

    // Estáticos
    inline float STATIC_NPC_MAX_SPEED;
    inline float STATIC_NPC_MAX_REVERSE_SPEED;
    inline float STATIC_NPC_ACCELERATION;
    inline float STATIC_NPC_CONTROL;
    inline float STATIC_NPC_HEALTH;
    inline float STATIC_NPC_FRICTION;
    inline float STATIC_NPC_WEIGHT;
    
    // Dinámicos
    inline float DYNAMIC_NPC_MAX_SPEED;
    inline float DYNAMIC_NPC_MAX_REVERSE_SPEED;
    inline float DYNAMIC_NPC_ACCELERATION;
    inline float DYNAMIC_NPC_CONTROL;
    inline float DYNAMIC_NPC_HEALTH;
    inline float DYNAMIC_NPC_FRICTION;
    inline float DYNAMIC_NPC_WEIGHT;
};
#endif //CONSTANTS_H
