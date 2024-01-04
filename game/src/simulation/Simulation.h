#ifndef SIMULATION_H
#define SIMULATION_H

#include "Particle.h"
#include "SimulationDef.h"

class Simulation {
public:
    bool paused;

    Particle parts[NPARTS];
    unsigned int pmap[ZRES][YRES][XRES];

    // TODO: have some sort of linked list to determine free particle spots
    int pfree;
    int maxId;

    Simulation();
    int create_part(coord_t x, coord_t y, coord_t z, ElementType type);
    void kill_part(coord_t id);

    void update();

    void move_behavior(int idx);
    void try_move(int idx, float x, float y, float z);
    void swap_part(coord_t x1, coord_t y1, coord_t z1, coord_t x2, coord_t y2, coord_t z2, int id1, int id2);

    bool raycast(coord_t x, coord_t y, coord_t z, float vx, float vy, float vz, coord_t &ox, coord_t &oy, coord_t &oz) const;

};

#endif