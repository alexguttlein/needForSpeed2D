#include "settings_loader.h"


float SettingsLoader::getFloat(const YAML::Node& node, const std::string& key) {
    if (!node[key]) {
        throw std::runtime_error("YAML Error: Missing float key: " + key);
    }
    return node[key].as<float>();
}


int SettingsLoader::getInt(const YAML::Node& node, const std::string& key) {
    if (!node[key]) {
        throw std::runtime_error("YAML Error: Missing int key: " + key);
    }
    return node[key].as<int>();
}


void SettingsLoader::load(const std::string& filepath) {
    try {
        YAML::Node config = YAML::LoadFile(filepath);

        // --- GAME CONFIG ---
        const auto& game_config = config["game_config"];
        Constants::MAX_PLAYERS_IN_GAME = getInt(game_config, "max_players_in_game");
        Constants::MAX_RACES = getInt(game_config, "max_races");

        // --- TIMING ---
        const auto& timing = config["timing"];
        Constants::THREAD_SLEEP_MS = getInt(timing, "thread_sleep_ms");
        Constants::TICKS_PER_SECOND = getInt(timing, "ticks_per_second");
        Constants::MAX_RACE_MINUTES = getInt(timing, "max_race_minutes");
        Constants::NOT_FINISH_PENALIZE_SECONDS = getInt(timing, "not_finish_penalize_seconds");
        Constants::UPGRADE_WAIT_SECONDS = getInt(timing, "upgrade_wait_seconds");
        
        // CÁLCULO DE VALORES DERIVADOS
        Constants::DT = 1.0f / static_cast<float>(Constants::TICKS_PER_SECOND);
        Constants::MAX_TICKS = Constants::MAX_RACE_MINUTES * 60 * Constants::TICKS_PER_SECOND;
        Constants::UPGRADE_WAIT_TICKS = Constants::UPGRADE_WAIT_SECONDS * Constants::TICKS_PER_SECOND;


        // --- UPGRADES & PENALTIES ---
        const auto& upgrades = config["upgrades"];
        Constants::DEFAULT_PENALIZE = getFloat(upgrades, "default_penalty_sec");
       
        const auto& health = upgrades["health"];
        Constants::HEALTH_UPGRADE_VALUE = getFloat(health, "value");
        Constants::PENALIZE_HEALTH_UPGRADE = getFloat(health, "penalty_sec");

        const auto& acceleration = upgrades["acceleration"];
        Constants::ACCELERATION_UPGRADE_VALUE = getFloat(acceleration, "value");
        Constants::PENALIZE_ACCELERATION_UPGRADE = getFloat(acceleration, "penalty_sec");

        const auto& control = upgrades["control"];
        Constants::CONTROL_UPGRADE_VALUE = getFloat(control, "value");
        Constants::PENALIZE_CONTROL_UPGRADE = getFloat(control, "penalty_sec");

        const auto& max_speed = upgrades["max_speed"];
        Constants::MAX_SPEED_UPGRADE_VALUE = getFloat(max_speed, "value");
        Constants::PENALIZE_SPEED_UPGRADE = getFloat(max_speed, "penalty_sec");

        // --- PHYSICS ---
        const auto& physics = config["physics_cars"];
        Constants::DEFAULT_RESTITUTION = getFloat(physics, "default_restitution");
        Constants::DEFAULT_LINEAR_DAMPING = getFloat(physics, "default_linear_damping");
        Constants::DEFAULT_ANGULAR_DAMPING = getFloat(physics, "default_angular_damping");
        Constants::DEFAULT_MULTIPLIER = getFloat(physics, "default_multiplier");
        
        Constants::NO_HEALTH = 0.0f;
        Constants::FRICTION_BASE = 0.0f;
        Constants::INITIAL_SPEED = 0.0f;

        // --- NPC ---
        const auto& npc = config["npc"];
        const auto& static_npc = npc["static"];
        Constants::STATIC_NPC_MAX_SPEED = getFloat(static_npc, "max_speed");
        Constants::STATIC_NPC_MAX_REVERSE_SPEED = getFloat(static_npc, "max_reverse_speed");
        Constants::STATIC_NPC_ACCELERATION = getFloat(static_npc, "acceleration");
        Constants::STATIC_NPC_CONTROL = getFloat(static_npc, "control");
        Constants::STATIC_NPC_HEALTH = getFloat(static_npc, "health");
        Constants::STATIC_NPC_FRICTION = getFloat(static_npc, "friction");
        Constants::STATIC_NPC_WEIGHT = getFloat(static_npc, "weight");

        const auto& dynamic_npc = npc["dynamic"];
        Constants::DYNAMIC_NPC_MAX_SPEED = getFloat(dynamic_npc, "max_speed");
        Constants::DYNAMIC_NPC_MAX_REVERSE_SPEED = getFloat(dynamic_npc, "max_reverse_speed");
        Constants::DYNAMIC_NPC_ACCELERATION = getFloat(dynamic_npc, "acceleration");
        Constants::DYNAMIC_NPC_CONTROL = getFloat(dynamic_npc, "control");
        Constants::DYNAMIC_NPC_HEALTH = getFloat(dynamic_npc, "health");
        Constants::DYNAMIC_NPC_FRICTION = getFloat(dynamic_npc, "friction");
        Constants::DYNAMIC_NPC_WEIGHT = getFloat(dynamic_npc, "weight");


        // --- AUTOS ---
        const auto& cars = config["cars"];
        
        const auto& ford = cars["ford"];
        Constants::FORD_BASE_ACCELERATION = getFloat(ford, "acceleration");
        Constants::FORD_BASE_CONTROL = getFloat(ford, "control");
        Constants::FORD_BASE_WEIGHT = getFloat(ford, "weight");
        Constants::FORD_BASE_MAX_SPEED = getFloat(ford, "max_speed");
        Constants::FORD_BASE_REVERSE_SPEED = getFloat(ford, "max_reverse_speed");
        Constants::FORD_BASE_HEALTH = getFloat(ford, "health");

        const auto& mazda = cars["mazda"];
        Constants::MAZDA_BASE_ACCELERATION = getFloat(mazda, "acceleration");
        Constants::MAZDA_BASE_CONTROL = getFloat(mazda, "control");
        Constants::MAZDA_BASE_WEIGHT = getFloat(mazda, "weight");
        Constants::MAZDA_BASE_MAX_SPEED = getFloat(mazda, "max_speed");
        Constants::MAZDA_BASE_REVERSE_SPEED = getFloat(mazda, "max_reverse_speed");
        Constants::MAZDA_BASE_HEALTH = getFloat(mazda, "health");

        const auto& corrolla = cars["corrolla"];
        Constants::CORROLLA_BASE_ACCELERATION = getFloat(corrolla, "acceleration");
        Constants::CORROLLA_BASE_CONTROL = getFloat(corrolla, "control");
        Constants::CORROLLA_BASE_WEIGHT = getFloat(corrolla, "weight");
        Constants::CORROLLA_BASE_MAX_SPEED = getFloat(corrolla, "max_speed");
        Constants::CORROLLA_BASE_REVERSE_SPEED = getFloat(corrolla, "max_reverse_speed");
        Constants::CORROLLA_BASE_HEALTH = getFloat(corrolla, "health");

        const auto& bmw = cars["bmw"];
        Constants::BMW_BASE_ACCELERATION = getFloat(bmw, "acceleration");
        Constants::BMW_BASE_CONTROL = getFloat(bmw, "control");
        Constants::BMW_BASE_WEIGHT = getFloat(bmw, "weight");
        Constants::BMW_BASE_MAX_SPEED = getFloat(bmw, "max_speed");
        Constants::BMW_BASE_REVERSE_SPEED = getFloat(bmw, "max_reverse_speed");
        Constants::BMW_BASE_HEALTH = getFloat(bmw, "health");
        
        const auto& jeep = cars["jeep"];
        Constants::JEEP_BASE_ACCELERATION = getFloat(jeep, "acceleration");
        Constants::JEEP_BASE_CONTROL = getFloat(jeep, "control");
        Constants::JEEP_BASE_WEIGHT = getFloat(jeep, "weight");
        Constants::JEEP_BASE_MAX_SPEED = getFloat(jeep, "max_speed");
        Constants::JEEP_BASE_REVERSE_SPEED = getFloat(jeep, "max_reverse_speed");
        Constants::JEEP_BASE_HEALTH = getFloat(jeep, "health");

        const auto& civic = cars["civic"];
        Constants::CIVIC_BASE_ACCELERATION = getFloat(civic, "acceleration");
        Constants::CIVIC_BASE_CONTROL = getFloat(civic, "control");
        Constants::CIVIC_BASE_WEIGHT = getFloat(civic, "weight");
        Constants::CIVIC_BASE_MAX_SPEED = getFloat(civic, "max_speed");
        Constants::CIVIC_BASE_REVERSE_SPEED = getFloat(civic, "max_reverse_speed");
        Constants::CIVIC_BASE_HEALTH = getFloat(civic, "health");

        const auto& truck = cars["truck"];
        Constants::TRUCK_BASE_ACCELERATION = getFloat(truck, "acceleration");
        Constants::TRUCK_BASE_CONTROL = getFloat(truck, "control");
        Constants::TRUCK_BASE_WEIGHT = getFloat(truck, "weight");
        Constants::TRUCK_BASE_MAX_SPEED = getFloat(truck, "max_speed");
        Constants::TRUCK_BASE_REVERSE_SPEED = getFloat(truck, "max_reverse_speed");
        Constants::TRUCK_BASE_HEALTH = getFloat(truck, "health");

        std::cout << "Configuración de juego cargada exitosamente desde: " << filepath << std::endl;
        
    } catch (const YAML::BadFile& e) {
        std::cerr << "Error al cargar YAML: Archivo no encontrado en " << filepath << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "Error al parsear YAML: " << e.what() << std::endl;
        throw;
    }
}
