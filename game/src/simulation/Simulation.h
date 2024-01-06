#ifndef SIMULATION_H
#define SIMULATION_H

#include "Particle.h"
#include "SimulationDef.h"
#include "Raycast.h"

class Simulation {
public:
    bool paused;

    Particle parts[NPARTS];
    uint32_t pmap[ZRES][YRES][XRES];
    uint32_t photons[ZRES][YRES][XRES];
    PartSwapBehavior can_move[ELEMENT_COUNT][ELEMENT_COUNT];

    // TODO: have some sort of linked list to determine free particle spots
    int pfree;
    int maxId;
    
    uint32_t parts_count;
    uint32_t frame_count; // Monotomic frame counter, will overflow in ~824 days @ 60 FPS. Do not keep the program open for this long

    Simulation();
    int create_part(const coord_t x, const coord_t y, const coord_t z, const ElementType type);
    void kill_part(const int id);

    void update();

    void move_behavior(const int idx);
    void try_move(const int idx, const float x, const float y, const float z,
        PartSwapBehavior behavior = PartSwapBehavior::NOT_EVALED_YET);
    void swap_part(const coord_t x1, const coord_t y1, const coord_t z1,
        const coord_t x2, const coord_t y2, const coord_t z2,
        const int id1, const int id2);

    bool raycast(const RaycastInput &in, RaycastOutput &out, auto pmapOccupied) const;
    PartSwapBehavior eval_move(const int idx, const coord_t nx, const coord_t ny, const coord_t nz) const;

private:
    void _init_can_move();
    void _raycast_movement(const int idx, const coord_t x, const coord_t y, const coord_t z);
};

#endif