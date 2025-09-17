#pragma once

#include "raylib.h"
#include "rlgl.h"

#include "SimulationDef.h"
#include "util/graphics/shader.h"
#include "util/types/persistent_buffer.h"
#include <atomic>

class Simulation;

constexpr unsigned int AIR_CELL_SIZE = 4;
static_assert(XRES % AIR_CELL_SIZE == 0, "XRES must be divisible by AIR_CELL_SIZE");
static_assert(YRES % AIR_CELL_SIZE == 0, "YRES must be divisible by AIR_CELL_SIZE");
static_assert(ZRES % AIR_CELL_SIZE == 0, "ZRES must be divisible by AIR_CELL_SIZE");

constexpr unsigned int AIR_XRES = XRES / AIR_CELL_SIZE;
constexpr unsigned int AIR_YRES = YRES / AIR_CELL_SIZE;
constexpr unsigned int AIR_ZRES = ZRES / AIR_CELL_SIZE;

constexpr float AIR_ADVECTION_DT             = 0.2;
constexpr float AIR_PRESSURE_VEL_COUPLING_DT = 0.2;
constexpr float AIR_VELOCITY_LOSS            = 0.99;
constexpr float AIR_PRESSURE_LOSS            = 0.99;
constexpr float MAX_AIR_VELOCITY             = 32;
constexpr float MAX_AIR_PRESSURE             = 128;

struct AirDelta {
    coord_t x, y, z;
    float dpressure;
};

class Air {
public:
    void init();
    void clear();
    void update();
    void wait_and_get();
    void upload();

    /// @brief Add pressure delta to (pixel) (x,y,z) coordinates
    void add_pv(const coord_t x, const coord_t y, const coord_t z, float diff);

    /// @brief Queue an out of band pressure diff (like add_pv), but run after sim.update()
    ///        That is when air sim is running on gpu so next download will overwrite any changes
    ///        This queues those changes for next frame, used in air tools
    void add_out_of_band_update(const coord_t x, const coord_t y, const coord_t z, float diff);

    Simulation &sim;
    explicit Air(Simulation &sim);

    bool enable = true;
    float vx[AIR_ZRES][AIR_YRES][AIR_XRES];
    float vy[AIR_ZRES][AIR_YRES][AIR_XRES];
    float vz[AIR_ZRES][AIR_YRES][AIR_XRES];
    float pv[AIR_ZRES][AIR_YRES][AIR_XRES];

    uint8_t wall_map[AIR_ZRES * AIR_YRES * AIR_XRES / 8 + 1];

    // ssbos, public for renderer
    static constexpr size_t SSBO_COUNT = 2;
    util::PersistentBuffer<SSBO_COUNT> ssbos_vx;
    util::PersistentBuffer<SSBO_COUNT> ssbos_vy;
    util::PersistentBuffer<SSBO_COUNT> ssbos_vz;
    util::PersistentBuffer<SSBO_COUNT> ssbos_pv;

    util::PersistentBuffer<SSBO_COUNT> ssbos_new_vx;
    util::PersistentBuffer<SSBO_COUNT> ssbos_new_vy;
    util::PersistentBuffer<SSBO_COUNT> ssbos_new_vz;
    util::PersistentBuffer<SSBO_COUNT> ssbos_new_pv;
    util::PersistentBuffer<1> ssbos_walls;

private:
    // shaders
    util::TPBComputeShader divergence_shader;
    util::TPBComputeShader advection_shader;
    util::TPBComputeShader pressure_from_vel_shader;
    util::TPBComputeShader pressure_blur_shader;
    util::TPBComputeShader vel_from_pressure_shader;
    bool changed_while_paused = false;

    // misc
    std::vector<AirDelta> out_of_band_air_updates;
    GLint iteration_uniform_loc;
    bool uploaded_once = false;

    friend Simulation;

    void solve_incompressibility();
    void fill_edges_and_advect_velocities();
};
