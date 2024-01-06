#ifndef SIMULATION_DEF_H
#define SIMULATION_DEF_H

#include "stdint.h"

using coord_t = uint8_t;
using signed_coord_t = int16_t;
using ElementType = unsigned int;

constexpr uint16_t ELEMENT_COUNT = __GLOBAL_ELEMENT_COUNT;

constexpr unsigned int XRES = 150;
constexpr unsigned int YRES = 100;
constexpr unsigned int ZRES = 150;
constexpr unsigned int NPARTS = XRES * YRES * ZRES;

constexpr float MAX_VELOCITY = 50.0f;

constexpr bool BOUNDS_CHECK(coord_t x, coord_t y, coord_t z) {
    return x > 0 && x <= XRES - 1 && y > 0 && y <= YRES - 1 && z > 0 && z <= ZRES - 1;
}
constexpr bool REVERSE_BOUNDS_CHECK(coord_t x, coord_t y, coord_t z) {
    return x < 1 || x >= XRES - 1 || y < 1 || y >= YRES - 1 || z < 1 || z >= ZRES - 1;
}

enum class PartSwapBehavior: uint8_t {
    NOOP = 0,
    SWAP = 1,
    OCCUPY_SAME = 2,
    SPECIAL = 3,
    NOT_EVALED_YET = 4 // Sentinel value, do not use in normal cases
};

#endif