#ifndef NPCDATA_H
#define NPCDATA_H

/*
* Estructura para almacenar datos de NPCs cargados desde YAML.
*
* */
struct NPCData {
    int id = 0;
    int carType = 0;
    float x = 0.0f;
    float y = 0.0f;
};
#endif // NPCDATA_H