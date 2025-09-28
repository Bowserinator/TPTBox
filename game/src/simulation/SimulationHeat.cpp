#include "SimulationHeat.h"
#include "Simulation.h"
#include "render/constants.h"
#include "util/common.h"
#include "util/graphics/shader.h"
#include "util/string.h"
#include "util/graphics/gl_time_query.h"

#include <cstring>
#include <glad.h>
#include <iostream>

void SimulationHeat::init() {
#include "../../resources/shaders/generated/heat.comp.h"
    heat_shader = util::TPBComputeShader(heat_comp_source);

    ssbo_constants = rlLoadShaderBuffer(sizeof(constants), NULL, RL_STATIC_READ);
    rlUpdateShaderBuffer(ssbo_constants, &constants, sizeof(constants), 0);

    ssbos_upload_download_dirty = util::PersistentBuffer<1>(GL_SHADER_STORAGE_BUFFER, sizeof(upload_download_dirty),
                                                            util::PBFlags::READ_AND_WRITE);
    ssbos_heat_conduct =
        util::PersistentBuffer<1>(GL_SHADER_STORAGE_BUFFER, sizeof(heat_conduct), util::PBFlags::WRITE);
    ssbos_data = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER, sizeof(heat_map), util::PBFlags::WRITE_ALT_READ);

    reset();
}

void SimulationHeat::reset() {
    uploaded_once = false;

    std::fill(util::t3d_begin(heat_map), util::t3d_end(heat_map), -1.0f);
    for (int i = 0; i < ssbos_data.get_buffer_count(); i++) {
        ssbos_data.wait(i);
        std::fill(&ssbos_data.get<float>(i)[0],
                  &ssbos_data.get<float>(i)[0] + (sizeof(heat_map) / sizeof(heat_map[0][0][0])), -1.0f);
        ssbos_data.lock(i);
    }

    memset(heat_conduct, 0, sizeof(heat_conduct));
    reset_dirty_chunks();
}

void SimulationHeat::upload(const uint32_t frame_count) {
    uploaded_once = true;
    ssbos_data.wait(0);
    ssbos_heat_conduct.wait(0);

#pragma omp parallel
    for (auto z = 0; z < ZRES; z++)
        for (auto y = 0; y < SIM_HEAT_YBLOCKS; y++) {
            if (upload_download_dirty[z * SIM_HEAT_YBLOCKS + y]) {
                int y_     = y * SIM_HEAT_DIRTY_BLOCK_SIZE; // Actual y value in [0, YRES)
                int bufIdx = z * (XRES * YRES) + y_ * XRES;

                std::copy(&heat_map[0][0][0] + bufIdx, &heat_map[0][0][0] + bufIdx + XRES * SIM_HEAT_DIRTY_BLOCK_SIZE,
                          &ssbos_data.get<float>(0)[0] + bufIdx);
                std::copy(&heat_conduct[0][0][0] + bufIdx,
                          &heat_conduct[0][0][0] + bufIdx + XRES * SIM_HEAT_DIRTY_BLOCK_SIZE,
                          &ssbos_heat_conduct.get<unsigned char>(0)[0] + bufIdx);
            }
        }
    ssbos_data.lock(0);
    ssbos_heat_conduct.lock(0);

    ssbos_upload_download_dirty.wait(0);
    memset(&ssbos_upload_download_dirty.get<uint32_t>(0)[0], 0, sizeof(upload_download_dirty));
    ssbos_upload_download_dirty.lock(0);

    // Write which indices are "active"
    int i = 0;
    for (int z = 0; z < SIM_HEAT_ZBLOCKS; z++)
        for (int y = 0; y < SIM_HEAT_YBLOCKS; y++)
            for (int x = 0; x < SIM_HEAT_XBLOCKS; x++) {
                if (!dirty_chunks[z][y][x]) continue;
                constants.DIRTY_INDICES[i++] = x + y * SIM_HEAT_XBLOCKS + z * SIM_HEAT_XBLOCKS * SIM_HEAT_YBLOCKS;
            }
    constants.DIRTY_INDEX_COUNT = i;
    constants.FRAME_COUNT       = frame_count;
    constants.IS_ENABLED        = 1;
    rlUpdateShaderBuffer(ssbo_constants, &constants, sizeof(constants), 0);
}

void SimulationHeat::dispatch(const uint32_t frame_count) {
    upload(frame_count);

    // Uncomment the two lines for timing the shader dispatch
    // util::GlTimeQuery query;

    rlEnableShader(heat_shader.id());
    rlBindShaderBuffer(ssbos_data.get_id(0), 0);
    rlBindShaderBuffer(ssbos_data.get_id(1), 1);
    rlBindShaderBuffer(ssbo_constants, 2);
    rlBindShaderBuffer(ssbos_upload_download_dirty.get_id(0), 3);
    rlBindShaderBuffer(ssbos_heat_conduct.get_id(0), 4);
    rlComputeShaderDispatch(constants.DIRTY_INDEX_COUNT, 1, 1);
    rlDisableShader();

    // std::cout << query.time_elapsed_ms() << " ms (heat sim)" << "\n";

    ssbos_data.lock(1);
}

void SimulationHeat::wait_and_get() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    ssbos_data.wait(1);
    ssbos_upload_download_dirty.wait(0);

    std::copy(&ssbos_upload_download_dirty.get<uint32_t>(1)[0],
              &ssbos_upload_download_dirty.get<uint32_t>(1)[0] + upload_download_dirty.size(),
              &upload_download_dirty[0]);

    // Fraction of dirty chunks
    unsigned int dirtyFracTop = 0;
    unsigned int dirtyFracBot = 0;

#pragma omp parallel for schedule(static) reduction(+ : dirtyFracTop) reduction(+ : dirtyFracBot)
    for (auto z = 0; z < ZRES; z++)
        for (auto y = 0; y < SIM_HEAT_YBLOCKS; y++) {
            dirtyFracBot++;
            if (upload_download_dirty[z * SIM_HEAT_YBLOCKS + y]) {
                dirtyFracTop++;
                int y_     = y * SIM_HEAT_DIRTY_BLOCK_SIZE; // Actual y value in [0, YRES)
                int bufIdx = z * (XRES * YRES) + y_ * XRES;

                std::copy(&ssbos_data.get<float>(1)[bufIdx],
                          &ssbos_data.get<float>(1)[bufIdx] + XRES * SIM_HEAT_DIRTY_BLOCK_SIZE, &heat_map[z][y_][0]);
            }
        }

    download_dirty_ratio = static_cast<double>(dirtyFracTop) / dirtyFracBot;
    ssbos_data.advance_cycle();
    ssbos_data.advance_cycle();
}

void SimulationHeat::reset_dirty_chunks() {
    memset(&upload_download_dirty[0], 0, sizeof(upload_download_dirty));
    memset(dirty_chunks, 0, sizeof(dirty_chunks));
}

void SimulationHeat::update_temperature(const coord_t x, const coord_t y, const coord_t z, const float temp,
                                        const unsigned char heat_cond) {
    heat_map[z][y][x]     = temp;
    heat_conduct[z][y][x] = heat_cond;
    flag_temp_update(x, y, z);
}

void SimulationHeat::flag_temp_update(const coord_t x, const coord_t y, const coord_t z) {
    upload_download_dirty[SIM_HEAT_YBLOCKS * z + y / SIM_HEAT_DIRTY_BLOCK_SIZE]                               = 1;
    dirty_chunks[z / SIM_HEAT_DIRTY_BLOCK_SIZE][y / SIM_HEAT_DIRTY_BLOCK_SIZE][x / SIM_HEAT_DIRTY_BLOCK_SIZE] = true;
}
