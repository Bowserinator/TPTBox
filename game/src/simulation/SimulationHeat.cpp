#include "SimulationHeat.h"
#include "Simulation.h"
#include "../util/types/gl_time_query.h"
#include "../render/constants.h"

#include <cstring>
#include <glad.h>
#include <iostream>

SimulationHeat::~SimulationHeat() {
    rlUnloadShaderProgram(heatProgram);
}

void SimulationHeat::init() {
#ifdef EMBED_SHADERS
    #include "../../resources/shaders/generated/heat.comp.h"
    heatShader = rlCompileShader(heat_comp_source, RL_COMPUTE_SHADER);
    heatProgram = rlLoadComputeShaderProgram(heatShader);
#else
    char * heatCode = LoadFileText("resources/shaders/heat.comp");
    heatShader = rlCompileShader(heatCode, RL_COMPUTE_SHADER);
    heatProgram = rlLoadComputeShaderProgram(heatShader);
    UnloadFileText(heatCode);
#endif
    
    ssboConstants = rlLoadShaderBuffer(sizeof(constants), NULL, RL_STATIC_READ);
    rlUpdateShaderBuffer(ssboConstants, &constants, sizeof(constants), 0);

    ssbosData = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER, sizeof(heat_map), util::PBFlags::WRITE_ALT_READ);
    std::fill(&heat_map[0][0][0], &heat_map[0][0][0] + (sizeof(heat_map) / sizeof(heat_map[0][0][0])), -1.0f);
    for (int i = 0; i < 6; i++) {
        std::fill(&ssbosData.get<float>(i)[0], &ssbosData.get<float>(i)[0] + (sizeof(heat_map) / sizeof(heat_map[0][0][0])), -1.0f);
        ssbosData.lock(i);
    }

    reset_dirty_chunks();
}

void SimulationHeat::dispatch() {
    uploadedOnce = true;

    ssbosData.wait(0);
    std::copy(
        &heat_map[0][0][0],
        &heat_map[0][0][0] + (sizeof(heat_map) / sizeof(heat_map[0][0][0])),
        &ssbosData.get<float>(0)[0]);
    ssbosData.lock(0);

    // Write which indices are "active"
    int i = 0;
    for (int z = 0; z < SIM_HEAT_ZBLOCKS; z++)
    for (int y = 0; y < SIM_HEAT_YBLOCKS; y++)
    for (int x = 0; x < SIM_HEAT_XBLOCKS; x++) {
        if (!dirty_chunks[z][y][x]) continue;
        constants.DIRTY_INDICES[i++] = x + y * SIM_HEAT_XBLOCKS + z * SIM_HEAT_XBLOCKS * SIM_HEAT_YBLOCKS;
    }
    constants.DIRTY_INDEX_COUNT = i;
    rlUpdateShaderBuffer(ssboConstants, &constants, sizeof(constants), 0);

    // Uncomment the two lines for timing the shader dispatch
    // util::GlTimeQuery query;

    rlEnableShader(heatProgram);
    rlBindShaderBuffer(ssbosData.getId(0), 0);
    rlBindShaderBuffer(ssbosData.getId(1), 1);
    rlBindShaderBuffer(ssboConstants, 2);
    rlComputeShaderDispatch(constants.DIRTY_INDEX_COUNT, 1, 1);
    rlDisableShader();

    // std::cout << query.timeElapsedMs() << " ms (heat sim)" << "\n";

    ssbosData.lock(1);
}

void SimulationHeat::wait_and_get() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    ssbosData.wait(1);

    std::copy(
        &ssbosData.get<float>(1)[0],
        &ssbosData.get<float>(1)[0] + (sizeof(heat_map) / sizeof(heat_map[0][0][0])),
        &heat_map[0][0][0]);

    ssbosData.advance_cycle();
    ssbosData.advance_cycle();
}

void SimulationHeat::reset_dirty_chunks() {
    memset(dirty_chunks, 0, sizeof(dirty_chunks));
}

void SimulationHeat::update_temperate(const coord_t x, const coord_t y, const coord_t z, const float temp) {
    heat_map[z][y][x] = temp;
    flag_temp_update(x, y, z);
}

void SimulationHeat::flag_temp_update(const coord_t x, const coord_t y, const coord_t z) {
    dirty_chunks[z / SIM_HEAT_ZRES][y / SIM_HEAT_YRES][x / SIM_HEAT_XRES] = true;
}
