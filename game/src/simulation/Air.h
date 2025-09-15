#pragma once

#include "raylib.h"
#include "rlgl.h"

#include "SimulationDef.h"
#include "util/graphics/shader.h"
#include "util/types/persistent_buffer.h"

class Simulation;

constexpr unsigned int AIR_CELL_SIZE = 4;
static_assert(XRES % AIR_CELL_SIZE == 0, "XRES must be divisible by AIR_CELL_SIZE");
static_assert(YRES % AIR_CELL_SIZE == 0, "YRES must be divisible by AIR_CELL_SIZE");
static_assert(ZRES % AIR_CELL_SIZE == 0, "ZRES must be divisible by AIR_CELL_SIZE");

constexpr unsigned int AIR_XRES = XRES / AIR_CELL_SIZE;
constexpr unsigned int AIR_YRES = YRES / AIR_CELL_SIZE;
constexpr unsigned int AIR_ZRES = ZRES / AIR_CELL_SIZE;

constexpr float AIR_ADVECTION_DT             = 5;
constexpr float AIR_PRESSURE_VEL_COUPLING_DT = 0.1;
constexpr float AIR_VELOCITY_LOSS            = 0.9999;
constexpr float AIR_PRESSURE_LOSS            = 0.9999;

class Air {
public:
    void init();
    void clear();
    void update();
    void wait_and_get();
    void upload();
    void explode(const coord_t x, const coord_t y, const coord_t z, float diff);

    Simulation &sim;
    explicit Air(Simulation &sim);

    float vx[AIR_ZRES][AIR_YRES][AIR_XRES];
    float vy[AIR_ZRES][AIR_YRES][AIR_XRES];
    float vz[AIR_ZRES][AIR_YRES][AIR_XRES];
    float pv[AIR_ZRES][AIR_YRES][AIR_XRES];

    uint8_t wall_map[AIR_ZRES * AIR_YRES * AIR_XRES / 8 + 1];

    util::PersistentBuffer<2> ssbos_vx;
    util::PersistentBuffer<2> ssbos_vy;
    util::PersistentBuffer<2> ssbos_vz;
    util::PersistentBuffer<2> ssbos_pv;

    util::PersistentBuffer<2> ssbos_vx2;
    util::PersistentBuffer<2> ssbos_vy2;
    util::PersistentBuffer<2> ssbos_vz2;

    util::PersistentBuffer<1> ssbos_walls;

private:
    util::TPBShader divergence_shader;
    util::TPBShader advection_shader;
    util::TPBShader pressure_from_vel_shader;
    util::TPBShader pressure_blur_shader;
    util::TPBShader vel_from_pressure_shader;
    GLint iteration_uniform_loc;

    void solve_incompressibility();
    void fill_edges_and_advect_velocities();
};
