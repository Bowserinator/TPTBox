#ifndef SIM_GRAPHICS_H
#define SIM_GRAPHICS_H

#include "SimulationDef.h"
#include <cmath>

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
#endif