#include "SimulationHeat.h"
#include "Simulation.h"
#include "../util/types/gl_time_query.h"

#include <cstring>
#include <glad.h>
#include <iostream>

SimulationHeat::~SimulationHeat() {
    rlUnloadShaderProgram(heatProgram);
}

void SimulationHeat::init() {
    char * heatCode = LoadFileText("resources/shaders/heat.comp");
    heatShader = rlCompileShader(heatCode, RL_COMPUTE_SHADER);
    heatProgram = rlLoadComputeShaderProgram(heatShader);
    UnloadFileText(heatCode);

    struct {
        int32_t SIMRES[4] = { (int)XRES, (int)YRES, (int)ZRES };
    } constants;
    ssboConstants = rlLoadShaderBuffer(sizeof(constants), NULL, RL_STATIC_READ);
    rlUpdateShaderBuffer(ssboConstants, &constants, sizeof(constants), 0);

    ssbosData = util::PersistentBuffer<6>(GL_SHADER_STORAGE_BUFFER, sizeof(heat_map), util::PBFlags::WRITE_ALT_READ);
    std::fill(&heat_map[0][0][0], &heat_map[0][0][0] + (sizeof(heat_map) / sizeof(heat_map[0][0][0])), -1.0f);
}

void SimulationHeat::dispatch() {
    uploadedOnce = true;

    ssbosData.wait(0);
    std::copy(
        &heat_map[0][0][0],
        &heat_map[0][0][0] + (sizeof(heat_map) / sizeof(heat_map[0][0][0])),
        &ssbosData.get<float>(0)[0]);
    ssbosData.lock(0);

    // Uncomment the two lines for timing the shader dispatch
    // util::GlTimeQuery query;

    rlEnableShader(heatProgram);
    rlBindShaderBuffer(ssbosData.getId(0), 0);
    rlBindShaderBuffer(ssbosData.getId(1), 1);
    rlBindShaderBuffer(ssboConstants, 2);
    rlComputeShaderDispatch(std::ceil(XRES / 10.0), std::ceil(YRES / 10.0), std::ceil(ZRES / 10.0));
    rlDisableShader();

    // std::cout << query.timeElapsedMs() << " ms (heat sim)" << "\n";

    ssbosData.lock(1);
}

void SimulationHeat::wait_and_get() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    ssbosData.wait(1);

    for (coord_t z = 1; z < ZRES - 1; z++) {
        const coord_t y1 = sim->min_y_per_zslice[z - 1];
        const coord_t y2 = sim->max_y_per_zslice[z - 1];
        const int offset1 = z * XRES * YRES + y1 * XRES;
        const int offset2 = z * XRES * YRES + (y2 + 1) * XRES;

        std::copy(
            &ssbosData.get<float>(1)[0] + offset1,
            &ssbosData.get<float>(1)[0] + offset2,
            &heat_map[0][0][0] + offset1);
    }

    ssbosData.advance_cycle();
    ssbosData.advance_cycle();
}
