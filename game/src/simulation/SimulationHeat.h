#ifndef SIMULATION_SIMULATIONHEAT_H_
#define SIMULATION_SIMULATIONHEAT_H_

#include "raylib.h"
#include "rlgl.h"
#include "../util/types/persistent_buffer.h"
#include "SimulationDef.h"

#include <array>
#include <cmath>

constexpr int SIM_HEAT_DIRTY_BLOCK_SIZE = 10; // For flagging updates
constexpr int SIM_HEAT_ZBLOCKS = (int)std::ceil((float)ZRES / SIM_HEAT_DIRTY_BLOCK_SIZE);
constexpr int SIM_HEAT_YBLOCKS = (int)std::ceil((float)YRES / SIM_HEAT_DIRTY_BLOCK_SIZE);
constexpr int SIM_HEAT_XBLOCKS = (int)std::ceil((float)XRES / SIM_HEAT_DIRTY_BLOCK_SIZE);

struct HeatConstants {
    int32_t SIMRES[4] = { (int)XRES, (int)YRES, (int)ZRES };
    int32_t COMPUTE_BLOCKS[4] = { SIM_HEAT_XBLOCKS, SIM_HEAT_YBLOCKS, SIM_HEAT_ZBLOCKS };
    uint32_t HEAT_BLOCK_SIZE = SIM_HEAT_DIRTY_BLOCK_SIZE;
    uint32_t DIRTY_INDEX_COUNT = 0;
    uint32_t DIRTY_INDICES[SIM_HEAT_ZBLOCKS * SIM_HEAT_YBLOCKS * SIM_HEAT_XBLOCKS];
};

class SimulationHeat {
public:
    ~SimulationHeat();

    float heat_map[ZRES][YRES][XRES]; // -1 indicates empty space
    bool uploadedOnce = false;        // Prevent download on first invocation

    void init();
    void reset();
    void dispatch();
    void wait_and_get();

    void update_temperate(const coord_t x, const coord_t y, const coord_t z, const float temp);
    void flag_temp_update(const coord_t x, const coord_t y, const coord_t z);
    void reset_dirty_chunks();

    inline unsigned int get_heat_in_ssbo() { return ssbosData.getId(0); }

private:
    util::PersistentBuffer<2> ssbosData;
    util::PersistentBuffer<1> ssbosUploadDownloadDirty;
    unsigned int ssboConstants;
    HeatConstants constants;

    unsigned int heatShader;
    unsigned int heatProgram;

    bool dirty_chunks[SIM_HEAT_ZBLOCKS][SIM_HEAT_YBLOCKS][SIM_HEAT_XBLOCKS];
    std::array<uint32_t, ZRES * SIM_HEAT_YBLOCKS> upload_download_dirty;
};

#endif // SIMULATION_SIMULATIONHEAT_H_
