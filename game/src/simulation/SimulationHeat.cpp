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

    ssbosUploadDownloadDirty = util::PersistentBuffer<1>(GL_SHADER_STORAGE_BUFFER,
        sizeof(upload_download_dirty), util::PBFlags::READ_AND_WRITE);
    ssbosHeatConduct = util::PersistentBuffer<1>(GL_SHADER_STORAGE_BUFFER,
        sizeof(heat_conduct), util::PBFlags::WRITE);
    ssbosData = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER, sizeof(heat_map), util::PBFlags::WRITE_ALT_READ);

    reset();
}

void SimulationHeat::reset() {
    uploaded_once = false;

    std::fill(&heat_map[0][0][0], &heat_map[0][0][0] + (sizeof(heat_map) / sizeof(heat_map[0][0][0])), -1.0f);
    for (int i = 0; i < ssbosData.getBufferCount(); i++) {
        ssbosData.wait(i);
        std::fill(&ssbosData.get<float>(i)[0],
            &ssbosData.get<float>(i)[0] + (sizeof(heat_map) / sizeof(heat_map[0][0][0])), -1.0f);
        ssbosData.lock(i);
    }

    memset(heat_conduct, 0, sizeof(heat_conduct));
    reset_dirty_chunks();
}

void SimulationHeat::upload(const uint32_t frame_count) {
    uploaded_once = true;
    ssbosData.wait(0);
    ssbosHeatConduct.wait(0);

    for (auto z = 0; z < ZRES; z++)
    for (auto y = 0; y < SIM_HEAT_YBLOCKS; y++) {
        if (upload_download_dirty[z * SIM_HEAT_YBLOCKS + y]) {
            int y_ = y * SIM_HEAT_DIRTY_BLOCK_SIZE; // Actual y value in [0, YRES)
            int bufIdx = z * (XRES * YRES) + y_ * XRES;

            std::copy(
                &heat_map[0][0][0] + bufIdx,
                &heat_map[0][0][0] + bufIdx + XRES * SIM_HEAT_DIRTY_BLOCK_SIZE,
                &ssbosData.get<float>(0)[0] + bufIdx
            );
            std::copy(
                &heat_conduct[0][0][0] + bufIdx,
                &heat_conduct[0][0][0] + bufIdx + XRES * SIM_HEAT_DIRTY_BLOCK_SIZE,
                &ssbosHeatConduct.get<unsigned char>(0)[0] + bufIdx
            );
        }
    }
    ssbosData.lock(0);
    ssbosHeatConduct.lock(0);

    ssbosUploadDownloadDirty.wait(0);
    memset(&ssbosUploadDownloadDirty.get<uint32_t>(0)[0], 0, sizeof(upload_download_dirty));
    ssbosUploadDownloadDirty.lock(0);

    // Write which indices are "active"
    int i = 0;
    for (int z = 0; z < SIM_HEAT_ZBLOCKS; z++)
    for (int y = 0; y < SIM_HEAT_YBLOCKS; y++)
    for (int x = 0; x < SIM_HEAT_XBLOCKS; x++) {
        if (!dirty_chunks[z][y][x]) continue;
        constants.DIRTY_INDICES[i++] = x + y * SIM_HEAT_XBLOCKS + z * SIM_HEAT_XBLOCKS * SIM_HEAT_YBLOCKS;
    }
    constants.DIRTY_INDEX_COUNT = i;
    constants.FRAME_COUNT = frame_count;
    rlUpdateShaderBuffer(ssboConstants, &constants, sizeof(constants), 0);
}

void SimulationHeat::dispatch(const uint32_t frame_count) {
    upload(frame_count);

    // Uncomment the two lines for timing the shader dispatch
    // util::GlTimeQuery query;

    rlEnableShader(heatProgram);
    rlBindShaderBuffer(ssbosData.getId(0), 0);
    rlBindShaderBuffer(ssbosData.getId(1), 1);
    rlBindShaderBuffer(ssboConstants, 2);
    rlBindShaderBuffer(ssbosUploadDownloadDirty.getId(0), 3);
    rlBindShaderBuffer(ssbosHeatConduct.getId(0), 4);
    rlComputeShaderDispatch(constants.DIRTY_INDEX_COUNT, 1, 1);
    rlDisableShader();

    // std::cout << query.timeElapsedMs() << " ms (heat sim)" << "\n";

    ssbosData.lock(1);
}

void SimulationHeat::wait_and_get() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    ssbosData.wait(1);
    ssbosUploadDownloadDirty.wait(0);

    std::copy(
        &ssbosUploadDownloadDirty.get<uint32_t>(1)[0],
        &ssbosUploadDownloadDirty.get<uint32_t>(1)[0] + upload_download_dirty.size(),
        &upload_download_dirty[0]);

    for (auto z = 0; z < ZRES; z++)
    for (auto y = 0; y < SIM_HEAT_YBLOCKS; y++) {
        if (upload_download_dirty[z * SIM_HEAT_YBLOCKS + y]) {
            int y_ = y * SIM_HEAT_DIRTY_BLOCK_SIZE; // Actual y value in [0, YRES)
            int bufIdx = z * (XRES * YRES) + y_ * XRES;

            std::copy(
                &ssbosData.get<float>(1)[bufIdx],
                &ssbosData.get<float>(1)[bufIdx] + XRES * SIM_HEAT_DIRTY_BLOCK_SIZE,
                &heat_map[z][y_][0]
            );
        }
    }
    memset(&upload_download_dirty[0], 0, sizeof(upload_download_dirty));

    ssbosData.advance_cycle();
    ssbosData.advance_cycle();
}

void SimulationHeat::reset_dirty_chunks() {
    memset(dirty_chunks, 0, sizeof(dirty_chunks));
}

void SimulationHeat::update_temperature(const coord_t x, const coord_t y, const coord_t z,
        const float temp, const unsigned char heat_cond) {
    heat_map[z][y][x] = temp;
    heat_conduct[z][y][x] = heat_cond;
    flag_temp_update(x, y, z);
}

void SimulationHeat::flag_temp_update(const coord_t x, const coord_t y, const coord_t z) {
    upload_download_dirty[SIM_HEAT_YBLOCKS * z + y / SIM_HEAT_DIRTY_BLOCK_SIZE] = 1;
    dirty_chunks[z / SIM_HEAT_DIRTY_BLOCK_SIZE][y / SIM_HEAT_DIRTY_BLOCK_SIZE][x / SIM_HEAT_DIRTY_BLOCK_SIZE] = true;
}
