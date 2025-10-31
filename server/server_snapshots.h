#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "../common/car.h"
#include "../common/queue.h"

#include <algorithm>


/*
* Por ahora el snapshot solo tiene el auto del jugador
*
* */
struct Snapshot{
    int posX;
    int posY;
};


class Snapshots{

private:

    Queue<Snapshot> snapshot_queue;
    std::mutex mtx;

public:

    /*
    * Constructor de Snapshots
    *
    * */
    explicit Snapshots(): snapshot_queue() {}


    /*
    * Agrega un snapshot a la queue de snapshots
    *
    * */
    void addSnapshot(const Snapshot& snapshot);


    /*
    * Extrae un snapshot de la queue de snapshots
    *
    * */
    bool popSnapshot(Snapshot& snapshot);

    /*
    * Cierra la queue de snapshots
    *
    * */
    void close();
    
    /*
    * Destructor de Snapshots
    *
    * */
    ~Snapshots();
}; 
#endif // SNAPSHOT_H
