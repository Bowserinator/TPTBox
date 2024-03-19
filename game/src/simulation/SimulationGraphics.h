#ifndef SIM_GRAPHICS_H
#define SIM_GRAPHICS_H

#include "SimulationDef.h"
#include "../util/types/heap_array.h"
#include "../util/types/bitset8.h"

#include <array>
#include <cmath>
#include <cstring>

constexpr unsigned int OCTREE_BLOCK_DEPTH = 6;
constexpr unsigned int OCTREE_BLOCK_DIM = 1 << OCTREE_BLOCK_DEPTH; // Sim split into blocks of this size, each of which is an octree
constexpr unsigned int X_BLOCKS = static_cast<unsigned int>(std::ceil(static_cast<float>(XRES) / OCTREE_BLOCK_DIM));
constexpr unsigned int Y_BLOCKS = static_cast<unsigned int>(std::ceil(static_cast<float>(YRES) / OCTREE_BLOCK_DIM));
constexpr unsigned int Z_BLOCKS = static_cast<unsigned int>(std::ceil(static_cast<float>(ZRES) / OCTREE_BLOCK_DIM));
// Size (arr el. count) of contigious element chunks to upload and diff at a time for color_data
constexpr unsigned int COLOR_DATA_CHUNK_SIZE = 16000; // Somewhat arbitrary
constexpr unsigned int COLOR_DATA_CHUNK_COUNT = static_cast<unsigned int>(std::ceil(
    (XRES * YRES * ZRES) / static_cast<float>(COLOR_DATA_CHUNK_SIZE)));

// Ambient occlusion counter block size
constexpr unsigned int AO_BLOCK_SIZE = 12;
constexpr unsigned int AO_X_BLOCKS = static_cast<unsigned int>(std::ceil(static_cast<float>(XRES) / AO_BLOCK_SIZE));
constexpr unsigned int AO_Y_BLOCKS = static_cast<unsigned int>(std::ceil(static_cast<float>(YRES) / AO_BLOCK_SIZE));
constexpr unsigned int AO_Z_BLOCKS = static_cast<unsigned int>(std::ceil(static_cast<float>(ZRES) / AO_BLOCK_SIZE));

constexpr uint32_t AO_FLAT_IDX(coord_t x, coord_t y, coord_t z) {
    return (x / AO_BLOCK_SIZE) + (y / AO_BLOCK_SIZE) * AO_X_BLOCKS + (z / AO_BLOCK_SIZE) * AO_X_BLOCKS * AO_Y_BLOCKS;
}

// Graphics are stored as 8 bit texture, so can only fit 8 bits
// Defaults should be when the flag = 0
namespace GraphicsFlagsIdx {
    constexpr uint8_t GLOW = 0;            // Glow (like fire)
    constexpr uint8_t BLUR = 1;            // Blur (like a gas)
    constexpr uint8_t REFRACT = 2;         // Refractive (mutually exclusive with reflective)
    constexpr uint8_t REFLECT = 3;         // Reflective (mutually exclusive with refractive)
    constexpr uint8_t NO_LIGHTING = 4;     // Unaffected by ambient occlusion & shadows, only works with static flags
}

// Preshifted
namespace GraphicsFlags {
    constexpr uint8_t GLOW = 1 << GraphicsFlagsIdx::GLOW;
    constexpr uint8_t BLUR = 1 << GraphicsFlagsIdx::BLUR;
    constexpr uint8_t REFRACT = 1 << GraphicsFlagsIdx::REFRACT;
    constexpr uint8_t REFLECT = 1 << GraphicsFlagsIdx::REFLECT;
    constexpr uint8_t NO_LIGHTING = 1 << GraphicsFlagsIdx::NO_LIGHTING;
}


class BitOctreeBlock;
struct SimulationGraphics {
    std::array<uint32_t, XRES * YRES * ZRES> color_data;
    std::array<uint8_t, XRES * YRES * ZRES> color_flags;
    std::array<bool, XRES * YRES * ZRES> color_force_update;
    std::array<uint8_t, COLOR_DATA_CHUNK_COUNT> color_data_modified;
    util::heap_array<BitOctreeBlock, X_BLOCKS * Y_BLOCKS * Z_BLOCKS> octree_blocks;
    std::array<int, AO_X_BLOCKS * AO_Y_BLOCKS * AO_Z_BLOCKS> ao_blocks;
    uint8_t shadow_map[SHADOW_MAP_Y][SHADOW_MAP_X];
    bool shadows_force_update;

    SimulationGraphics() {
        reset();
    }

    void reset() {
        shadows_force_update = false;
        color_data.fill(0);
        color_flags.fill(0);
        color_force_update.fill(false);
        ao_blocks.fill(0);
        color_data_modified.fill(0xFF);
        memset(&shadow_map[0][0], 0, sizeof(shadow_map));
    }
};

#endif