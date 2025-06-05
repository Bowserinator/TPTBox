#ifndef SIMULATION_AIR_H_
#define SIMULATION_AIR_H_

#include "raylib.h"
#include "rlgl.h"

#include "SimulationDef.h"
#include "util/types/persistent_buffer.h"

class Simulation;

constexpr unsigned int AIR_CELL_SIZE = 4;
static_assert(XRES % AIR_CELL_SIZE == 0, "XRES must be divisible by AIR_CELL_SIZE");
static_assert(YRES % AIR_CELL_SIZE == 0, "YRES must be divisible by AIR_CELL_SIZE");
static_assert(ZRES % AIR_CELL_SIZE == 0, "ZRES must be divisible by AIR_CELL_SIZE");

constexpr unsigned int AIR_XRES = XRES / AIR_CELL_SIZE;
constexpr unsigned int AIR_YRES = YRES / AIR_CELL_SIZE;
constexpr unsigned int AIR_ZRES = ZRES / AIR_CELL_SIZE;

struct AirConstants {
    int32_t AIRRES[4] = { (int)AIR_XRES, (int)AIR_YRES, (int)AIR_ZRES };
    uint32_t CELL_SIZE = AIR_CELL_SIZE;
};

class Air {
public:
    void init();
    void clear();
    void update();
    void wait_and_get();

    Simulation & sim;
    explicit Air(Simulation & sim);

    float vx[AIR_ZRES][AIR_YRES][AIR_XRES];
    float vy[AIR_ZRES][AIR_YRES][AIR_XRES];
    float vz[AIR_ZRES][AIR_YRES][AIR_XRES];

    uint8_t wall_map[AIR_ZRES * AIR_YRES * AIR_XRES / 8 + 1];

    util::PersistentBuffer<2> ssbos_vx;
    util::PersistentBuffer<2> ssbos_vy;
    util::PersistentBuffer<2> ssbos_vz;

    util::PersistentBuffer<2> ssbos_vx2;
    util::PersistentBuffer<2> ssbos_vy2;
    util::PersistentBuffer<2> ssbos_vz2;

    util::PersistentBuffer<1> ssbos_walls;

private:
    unsigned int ssbo_constants;
    AirConstants constants;

    unsigned int divergence_program;
    unsigned int advection_program;
    GLint iteration_uniform_loc;

    void solve_incompressibility();
    void fill_edges_and_advect_velocities();
};

#endif // SIMULATION_AIR_H_
