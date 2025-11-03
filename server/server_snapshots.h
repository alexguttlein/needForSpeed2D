#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "../common/gameSnapshot.h"
#include "../common/queue.h"

#include <algorithm>


/*
* queda como clase, mas tarde deberia implementarse logica de manejo de snapshots
* como almacenamiento limitado, descartado de viejos, etc.
* */


class WorldSnapshots{

private:

    Queue<GameSnapshot> snapshot_queue;

public:

    /*
    * Constructor de WorldSnapshots
    *
    * */
    explicit WorldSnapshots(): snapshot_queue() {}


    /*
    * Agrega un snapshot a la queue de snapshots
    *
    * */
    void addSnapshot(const GameSnapshot& snapshot);


    /*
    * Extrae un snapshot de la queue de snapshots
    *
    * */
    bool popSnapshot(GameSnapshot& snapshot);

    /*
    * Cierra la queue de snapshots
    *
    * */
    void close();
    
    /*
    * Destructor de Snapshots
    *
    * */
    ~WorldSnapshots();
}; 
#endif // SNAPSHOT_H
