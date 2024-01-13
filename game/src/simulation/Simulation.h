#ifndef SIMULATION_H
#define SIMULATION_H

#include "Particle.h"
#include "SimulationDef.h"
#include "Raycast.h"
#include "Air.h"

#include "../util/rand.h"
#include <vector>

enum class GravityMode {
    VERTICAL = 0,
    ZERO_G = 1,
    RADIAL = 2
};

class Simulation {
public:
    bool paused;
    GravityMode gravity_mode;

    Particle parts[NPARTS];
    pmap_id pmap[ZRES][YRES][XRES];
    pmap_id photons[ZRES][YRES][XRES];
    PartSwapBehavior can_move[ELEMENT_COUNT + 1][ELEMENT_COUNT + 1];

    Air air;

    part_id pfree;
    part_id maxId;
    
    uint32_t parts_count;
    uint32_t frame_count; // Monotomic frame counter, will overflow in ~824 days @ 60 FPS. Do not keep the program open for this long

    unsigned int sim_thread_count;
    unsigned int actual_thread_count;
    RNG rng;


    Simulation();
    part_id create_part(const coord_t x, const coord_t y, const coord_t z, const ElementType type);
    void kill_part(const part_id id);

    void update();
    void update_zslice(const coord_t zslice);
    void recalc_free_particles();

    void update_part(const part_id i);

    void move_behavior(const part_id idx);
    void try_move(const part_id idx, const float x, const float y, const float z,
        PartSwapBehavior behavior = PartSwapBehavior::NOT_EVALED_YET);
    void swap_part(const coord_t x1, const coord_t y1, const coord_t z1,
        const coord_t x2, const coord_t y2, const coord_t z2,
        const part_id id1, const part_id id2);

    bool raycast(const RaycastInput &in, RaycastOutput &out, const auto &pmapOccupied) const;
    PartSwapBehavior eval_move(const part_id idx, const coord_t nx, const coord_t ny, const coord_t nz) const;

private:
    void _init_can_move();
    void _raycast_movement(const part_id idx, const coord_t x, const coord_t y, const coord_t z);
};

#endif