#include "Air.h"
#include "util/types/gl_time_query.h"
#include "rlgl.h"

#include <algorithm>
#include <memory>
#include <iostream>
#include <cstring>
#include <glad.h>
#include <cmath>

Air::Air(Simulation &sim): sim(sim) {}

void Air::init() {
#ifdef EMBED_SHADERS
    #include "../../resources/shaders/generated/air_divergence.comp.h"
    #include "../../resources/shaders/generated/air_advection.comp.h"

    unsigned int div_shader = rlCompileShader(air_divergence_comp_source, RL_COMPUTE_SHADER);
    divergence_program = rlLoadComputeShaderProgram(div_shader);
    unsigned int adv_shader = rlCompileShader(air_advection_comp_source, RL_COMPUTE_SHADER);
    adv_program = rlLoadComputeShaderProgram(adv_shader);
#else
    char * div_code = LoadFileText("resources/shaders/air_divergence.comp");
    unsigned int div_shader = rlCompileShader(div_code, RL_COMPUTE_SHADER);
    divergence_program = rlLoadComputeShaderProgram(div_shader);
    UnloadFileText(div_code);

    char * adv_code = LoadFileText("resources/shaders/air_advection.comp");
    unsigned int adv_shader = rlCompileShader(adv_code, RL_COMPUTE_SHADER);
    advection_program = rlLoadComputeShaderProgram(adv_shader);
    UnloadFileText(adv_code);
#endif

    ssbo_constants = rlLoadShaderBuffer(sizeof(constants), NULL, RL_STATIC_READ);
    rlUpdateShaderBuffer(ssbo_constants, &constants, sizeof(constants), 0);

    ssbos_vx = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER,
        sizeof(vx), util::PBFlags::READ_AND_WRITE);
    ssbos_vy = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER,
        sizeof(vy), util::PBFlags::READ_AND_WRITE);
    ssbos_vz = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER,
        sizeof(vz), util::PBFlags::READ_AND_WRITE);
    ssbos_walls = util::PersistentBuffer<1>(GL_SHADER_STORAGE_BUFFER,
        sizeof(wall_map), util::PBFlags::WRITE);

    iteration_uniform_loc = glGetUniformLocation(divergence_program, "iteration");
    clear();
}

void Air::clear() {
    memset(vx, 0.0f, sizeof(vx));
    memset(vy, 0.0f, sizeof(vy));
    memset(vz, 0.0f, sizeof(vz));
    memset(wall_map, 0, sizeof(wall_map));

    for (auto i = 0; i < ssbos_vx.getBufferCount(); i++) {
        ssbos_vx.wait(i);
        ssbos_vy.wait(i);
        ssbos_vz.wait(i);

        std::fill(&ssbos_vx.get<float>(i)[0],
            &ssbos_vx.get<float>(i)[0] + (sizeof(vx) / sizeof(vz[0][0][0])), 0.0f);
        std::fill(&ssbos_vy.get<float>(i)[0],
            &ssbos_vy.get<float>(i)[0] + (sizeof(vy) / sizeof(vy[0][0][0])), 0.0f);
        std::fill(&ssbos_vz.get<float>(i)[0],
            &ssbos_vz.get<float>(i)[0] + (sizeof(vz) / sizeof(vz[0][0][0])), 0.0f);

        ssbos_vx.lock(i);
        ssbos_vy.lock(i);
        ssbos_vz.lock(i);
    }
}

void Air::update() {
    //for (int x = 0; x < AIR_XRES; x++)
    //for (int z = 0; z < AIR_ZRES; z++)
    //    wall_map[(x + z * AIR_XRES * AIR_YRES + 2 * AIR_XRES) / 8] = 0xFF;

    memcpy(ssbos_walls.get<uint8_t>(0), wall_map, sizeof(wall_map)); // TODO diff

    solve_incompressibility();
    fill_edges_and_advect_velocities();
}

void Air::solve_incompressibility() {
    rlEnableShader(divergence_program);
    rlBindShaderBuffer(ssbos_vx.getId(0), 0);
    rlBindShaderBuffer(ssbos_vy.getId(0), 1);
    rlBindShaderBuffer(ssbos_vz.getId(0), 2);
    rlBindShaderBuffer(ssbo_constants, 3);
    rlBindShaderBuffer(ssbos_walls.getId(0), 4);

    // util::GlTimeQuery query;

    constexpr int DIVERGENCE_REMOVING_ITERATIONS = 3;
    for (int i = 0; i < DIVERGENCE_REMOVING_ITERATIONS; i++) {
        glUniform1iv(iteration_uniform_loc, 1, &i);

        rlComputeShaderDispatch(
            std::ceil((AIR_XRES - 2.0f) / 8.0f),
            std::ceil((AIR_YRES - 2.0f) / 8.0f),
            std::ceil((AIR_ZRES - 2.0f) / 16.0f));
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    rlDisableShader();

    // std::cout << query.timeElapsedMs() << " ms (air sim)" << "\n";
}

void Air::fill_edges_and_advect_velocities() {
    rlEnableShader(advection_program);
    rlBindShaderBuffer(ssbos_vx.getId(0), 0);
    rlBindShaderBuffer(ssbos_vy.getId(0), 1);
    rlBindShaderBuffer(ssbos_vz.getId(0), 2);
    rlBindShaderBuffer(ssbo_constants, 3);
    rlBindShaderBuffer(ssbos_vx.getId(1), 4);
    rlBindShaderBuffer(ssbos_vy.getId(1), 5);
    rlBindShaderBuffer(ssbos_vz.getId(1), 6);
    rlBindShaderBuffer(ssbos_walls.getId(0), 7);

    // util::GlTimeQuery query;

    rlComputeShaderDispatch(
        std::ceil((AIR_XRES - 2.0f) / 8.0f),
        std::ceil((AIR_YRES - 2.0f) / 8.0f),
        std::ceil((AIR_ZRES - 2.0f) / 16.0f));
    rlDisableShader();

    // std::cout << query.timeElapsedMs() << " ms (air sim - advection)" << "\n";
}

void Air::wait_and_get() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    ssbos_vx.wait(1);
    ssbos_vy.wait(1);
    ssbos_vz.wait(1);

    memcpy(&vx[0], &ssbos_vx.get<float>(1)[0], sizeof(vx));
    memcpy(&vy[0], &ssbos_vy.get<float>(1)[0], sizeof(vy));
    memcpy(&vz[0], &ssbos_vz.get<float>(1)[0], sizeof(vz));

    ssbos_vx.advance_cycle();
    ssbos_vy.advance_cycle();
    ssbos_vz.advance_cycle();
}
