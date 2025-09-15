#include "Air.h"
#include "rlgl.h"
#include "util/graphics/shader.h"
#include "util/string.h"
#include "util/types/gl_time_query.h"

#include <cmath>
#include <cstring>
#include <glad.h>

/// Air compute shaders in 8 8 16 = 1024
void dispatch_air_compute_shaders() {
    rlComputeShaderDispatch(std::ceil((AIR_XRES - 2.0f) / 8.0f), std::ceil((AIR_YRES - 2.0f) / 8.0f),
                        std::ceil((AIR_ZRES - 2.0f) / 16.0f));
}

Air::Air(Simulation &sim) : sim(sim) {}

void Air::init() {
#include "../../resources/shaders/generated/air_advection.comp.h"
#include "../../resources/shaders/generated/air_blur.comp.h"
#include "../../resources/shaders/generated/air_divergence.comp.h"
#include "../../resources/shaders/generated/air_pressure_from_vel.comp.h"
#include "../../resources/shaders/generated/air_vel_from_pressure.comp.h"

    air_pressure_from_vel_comp_source.add_const("dt", AIR_PRESSURE_VEL_COUPLING_DT)
        .add_const("AIR_PRESSURE_LOSS", AIR_PRESSURE_LOSS);
    air_vel_from_pressure_comp_source.add_const("dt", AIR_PRESSURE_VEL_COUPLING_DT)
        .add_const("AIR_VELOCITY_LOSS", AIR_VELOCITY_LOSS);
    air_advection_comp_source.add_const("dt", AIR_ADVECTION_DT);

    std::reference_wrapper<util::TPBShaderSourceCode> srcs[] = {
        air_advection_comp_source, air_divergence_comp_source, air_pressure_from_vel_comp_source,
        air_vel_from_pressure_comp_source, air_blur_comp_source};
    for (auto &src : srcs)
        src.get()
            .add_const("CELL_SIZE", AIR_CELL_SIZE | as<uint32_t>)
            .add_const_arr<int32_t, 4>("AIRRES", {AIR_XRES, AIR_YRES, AIR_ZRES, 0})
            .update();

    advection_shader         = util::TPBComputeShader{air_advection_comp_source};
    divergence_shader        = util::TPBComputeShader{air_divergence_comp_source};
    pressure_from_vel_shader = util::TPBComputeShader{air_pressure_from_vel_comp_source};
    vel_from_pressure_shader = util::TPBComputeShader{air_vel_from_pressure_comp_source};
    pressure_blur_shader     = util::TPBComputeShader{air_blur_comp_source};

    ssbos_vx    = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER, sizeof(vx), util::PBFlags::READ_AND_WRITE);
    ssbos_vy    = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER, sizeof(vy), util::PBFlags::READ_AND_WRITE);
    ssbos_vz    = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER, sizeof(vz), util::PBFlags::READ_AND_WRITE);
    ssbos_pv    = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER, sizeof(pv), util::PBFlags::READ_AND_WRITE);
    ssbos_walls = util::PersistentBuffer<1>(GL_SHADER_STORAGE_BUFFER, sizeof(wall_map), util::PBFlags::WRITE);

    iteration_uniform_loc = glGetUniformLocation(divergence_shader.id(), "iteration");
    clear();
}

void Air::clear() {
    memset(vx, 0.0f, sizeof(vx));
    memset(vy, 0.0f, sizeof(vy));
    memset(vz, 0.0f, sizeof(vz));
    memset(pv, 0.0f, sizeof(pv));
    memset(wall_map, 0, sizeof(wall_map));

    for (auto i = 0; i < ssbos_vx.getBufferCount(); i++) {
        ssbos_vx.wait(i);
        ssbos_vy.wait(i);
        ssbos_vz.wait(i);
        ssbos_pv.wait(i);

        std::fill(&ssbos_vx.get<float>(i)[0], &ssbos_vx.get<float>(i)[0] + (sizeof(vx) / sizeof(vz[0][0][0])), 0.0f);
        std::fill(&ssbos_vy.get<float>(i)[0], &ssbos_vy.get<float>(i)[0] + (sizeof(vy) / sizeof(vy[0][0][0])), 0.0f);
        std::fill(&ssbos_vz.get<float>(i)[0], &ssbos_vz.get<float>(i)[0] + (sizeof(vz) / sizeof(vz[0][0][0])), 0.0f);
        std::fill(&ssbos_pv.get<float>(i)[0], &ssbos_pv.get<float>(i)[0] + (sizeof(pv) / sizeof(pv[0][0][0])), 0.0f);

        ssbos_vx.lock(i);
        ssbos_vy.lock(i);
        ssbos_vz.lock(i);
        ssbos_pv.lock(i);
    }
}

void Air::update() {
    // for (int x = 0; x < AIR_XRES; x++)
    // for (int z = 0; z < AIR_ZRES; z++)
    //     wall_map[(x + z * AIR_XRES * AIR_YRES + 2 * AIR_XRES) / 8] = 0xFF;

    memcpy(ssbos_walls.get<uint8_t>(0), wall_map, sizeof(wall_map)); // TODO diff

    // Pressure update
    rlEnableShader(pressure_from_vel_shader.id());
    rlBindShaderBuffer(ssbos_vx.getId(0), 0);
    rlBindShaderBuffer(ssbos_vy.getId(0), 1);
    rlBindShaderBuffer(ssbos_vz.getId(0), 2);
    rlBindShaderBuffer(ssbos_pv.getId(0), 3);
    dispatch_air_compute_shaders();
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    rlDisableShader();

    // Blur velocity and pressure fields
    // Can technically only blur pressure but spherical pressure waves have
    // less axis aligned artifacts when we also blur velocity (adds viscosity)
    rlEnableShader(pressure_blur_shader.id());
    for (const auto &ssbo : {std::ref(ssbos_pv), std::ref(ssbos_vx), std::ref(ssbos_vy), std::ref(ssbos_vz)}) {
        rlBindShaderBuffer(ssbo.get().getId(0), 0);
        rlBindShaderBuffer(ssbo.get().getId(1), 1);
        dispatch_air_compute_shaders();
        ssbo.get().advance_cycle();
    }
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    rlDisableShader();

    // Vel update
    rlEnableShader(vel_from_pressure_shader.id());
    rlBindShaderBuffer(ssbos_vx.getId(0), 0);
    rlBindShaderBuffer(ssbos_vy.getId(0), 1);
    rlBindShaderBuffer(ssbos_vz.getId(0), 2);
    rlBindShaderBuffer(ssbos_walls.getId(0), 3);
    rlBindShaderBuffer(ssbos_pv.getId(0), 4);
    dispatch_air_compute_shaders();
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    rlDisableShader();

    // The rest
    solve_incompressibility();
    fill_edges_and_advect_velocities();
}

void Air::upload() {
    ssbos_vx.advance_cycle();
    ssbos_vy.advance_cycle();
    ssbos_vz.advance_cycle();
    ssbos_pv.advance_cycle();

    ssbos_vx.wait(0);
    ssbos_vy.wait(0);
    ssbos_vz.wait(0);
    ssbos_pv.wait(0);

    memcpy(&ssbos_vx.get<float>(0)[0], &vx[0], sizeof(vx));
    memcpy(&ssbos_vy.get<float>(0)[0], &vy[0], sizeof(vy));
    memcpy(&ssbos_vz.get<float>(0)[0], &vz[0], sizeof(vz));
    memcpy(&ssbos_pv.get<float>(0)[0], &pv[0], sizeof(pv));

    ssbos_vx.lock(0);
    ssbos_vy.lock(0);
    ssbos_vz.lock(0);
    ssbos_pv.lock(0);
}

void Air::explode(const coord_t x, const coord_t y, const coord_t z, float diff) {
    // TODO: remove??
    vx[z / AIR_CELL_SIZE][y / AIR_CELL_SIZE][x / AIR_CELL_SIZE] -= diff;
    vx[z / AIR_CELL_SIZE][y / AIR_CELL_SIZE][x / AIR_CELL_SIZE + 1] += diff;
    vy[z / AIR_CELL_SIZE][y / AIR_CELL_SIZE][x / AIR_CELL_SIZE] -= diff;
    vy[z / AIR_CELL_SIZE][y / AIR_CELL_SIZE + 1][x / AIR_CELL_SIZE] += diff;
    vz[z / AIR_CELL_SIZE][y / AIR_CELL_SIZE][x / AIR_CELL_SIZE] -= diff;
    vz[z / AIR_CELL_SIZE + 1][y / AIR_CELL_SIZE][x / AIR_CELL_SIZE] += diff;
}

void Air::solve_incompressibility() {
    rlEnableShader(divergence_shader.id());
    rlBindShaderBuffer(ssbos_vx.getId(0), 0);
    rlBindShaderBuffer(ssbos_vy.getId(0), 1);
    rlBindShaderBuffer(ssbos_vz.getId(0), 2);
    rlBindShaderBuffer(ssbos_walls.getId(0), 4);

    // util::GlTimeQuery query;
    constexpr int DIVERGENCE_REMOVING_ITERATIONS = 4;
    for (int i = 0; i < DIVERGENCE_REMOVING_ITERATIONS; i++) {
        glUniform1iv(iteration_uniform_loc, 1, &i);
        dispatch_air_compute_shaders();
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
    rlDisableShader();
    // std::cout << query.timeElapsedMs() << " ms (air sim)" << "\n";
}

void Air::fill_edges_and_advect_velocities() {
    rlEnableShader(advection_shader.id());
    rlBindShaderBuffer(ssbos_vx.getId(0), 0);
    rlBindShaderBuffer(ssbos_vy.getId(0), 1);
    rlBindShaderBuffer(ssbos_vz.getId(0), 2);
    rlBindShaderBuffer(ssbos_vx.getId(1), 4);
    rlBindShaderBuffer(ssbos_vy.getId(1), 5);
    rlBindShaderBuffer(ssbos_vz.getId(1), 6);
    rlBindShaderBuffer(ssbos_walls.getId(0), 7);
    rlBindShaderBuffer(ssbos_pv.getId(0), 8);

    // util::GlTimeQuery query;
    dispatch_air_compute_shaders();
    rlDisableShader();
    // std::cout << query.timeElapsedMs() << " ms (air sim - advection)" << "\n";
}

void Air::wait_and_get() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    ssbos_vx.wait(1);
    ssbos_vy.wait(1);
    ssbos_vz.wait(1);
    ssbos_pv.wait(0);

    memcpy(&vx[0], &ssbos_vx.get<float>(1)[0], sizeof(vx));
    memcpy(&vy[0], &ssbos_vy.get<float>(1)[0], sizeof(vy));
    memcpy(&vz[0], &ssbos_vz.get<float>(1)[0], sizeof(vz));
    memcpy(&pv[0], &ssbos_pv.get<float>(0)[0], sizeof(pv));

    ssbos_vx.advance_cycle();
    ssbos_vy.advance_cycle();
    ssbos_vz.advance_cycle();
    ssbos_pv.advance_cycle();
}
