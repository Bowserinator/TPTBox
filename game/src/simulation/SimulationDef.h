#ifndef SIMULATION_DEF_H
#define SIMULATION_DEF_H

#include "stdint.h"
#include <cmath>

using coord_t = uint32_t;
using signed_coord_t = int32_t;
using ElementType = unsigned int;

// Must be signed, negative values are used for pfree
// and error codes in some functions
using part_id = int32_t;
using pmap_id = int32_t; // This is ID and type merged into 1 32 bit value

// Pmap and photons store positive ids in this format:
// [TYP 10 bits][ID 22 bits]
constexpr uint32_t PMAP_ID_BITS = 22;
constexpr uint16_t PT_NUM = 1 << (32 - PMAP_ID_BITS); // Max number of elements possible

#define ID(r) (r & ((1 << PMAP_ID_BITS) - 1))
#define TYP(r) (r >> PMAP_ID_BITS)
#define PMAP(t, i) (((uint32_t)t << PMAP_ID_BITS) | (uint32_t)i)

constexpr uint16_t ELEMENT_COUNT = __GLOBAL_ELEMENT_COUNT;

constexpr unsigned int XRES = 200;
constexpr unsigned int YRES = 200;
constexpr unsigned int ZRES = 200;
constexpr unsigned int NPARTS = XRES * YRES * ZRES;

constexpr float MAX_VELOCITY = 50.0f;

constexpr bool BOUNDS_CHECK(coord_t x, coord_t y, coord_t z) {
    return x > 0 && x < XRES - 1 && y > 0 && y < YRES - 1 && z > 0 && z < ZRES - 1;
}
constexpr bool REVERSE_BOUNDS_CHECK(coord_t x, coord_t y, coord_t z) {
    return x < 1 || x >= XRES - 1 || y < 1 || y >= YRES - 1 || z < 1 || z >= ZRES - 1;
}
constexpr uint32_t FLAT_IDX(coord_t x, coord_t y, coord_t z) {
    return x + y * XRES + z * (XRES * YRES);
}

enum class PartSwapBehavior: uint8_t {
    NOOP = 0,
    SWAP = 1,
    OCCUPY_SAME = 2,
    SPECIAL = 3,
    NOT_EVALED_YET = 4 // Sentinel value, do not use in normal cases
};

namespace PartErr {
    constexpr part_id ALREADY_OCCUPIED = -1;
    constexpr part_id PARTS_FULL = -3;
};

constexpr unsigned int OCTREE_BLOCK_DEPTH = 6;
constexpr unsigned int OCTREE_BLOCK_DIM = 1 << OCTREE_BLOCK_DEPTH; // Sim split into blocks of this size, each of which is an octree
constexpr unsigned int X_BLOCKS = static_cast<unsigned int>(std::ceil(static_cast<float>(XRES) / OCTREE_BLOCK_DIM));
constexpr unsigned int Y_BLOCKS = static_cast<unsigned int>(std::ceil(static_cast<float>(YRES) / OCTREE_BLOCK_DIM));
constexpr unsigned int Z_BLOCKS = static_cast<unsigned int>(std::ceil(static_cast<float>(ZRES) / OCTREE_BLOCK_DIM));
// Size (arr el. count) of contigious element chunks to upload and diff at a time for color_data
constexpr unsigned int COLOR_DATA_CHUNK_SIZE = 16384; // Somewhat arbitrary
constexpr unsigned int COLOR_DATA_CHUNK_COUNT = static_cast<unsigned int>(std::ceil(
    (XRES * YRES * ZRES) / static_cast<float>(COLOR_DATA_CHUNK_SIZE)));

#endif