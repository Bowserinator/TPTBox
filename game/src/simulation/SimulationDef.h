#ifndef SIMULATION_DEF_H
#define SIMULATION_DEF_H

#include "stdint.h"

using coord_t = uint8_t;
using ElementType = unsigned int;

constexpr unsigned int XRES = 100;
constexpr unsigned int YRES = 100;
constexpr unsigned int ZRES = 100;
constexpr unsigned int NPARTS = XRES * YRES * ZRES;

constexpr float MAX_VELOCITY = 50.0f;

constexpr bool BOUNDS_CHECK(coord_t x, coord_t y, coord_t z) {
    return x > 0 && x <= XRES - 1 && y > 0 && y <= YRES - 1 && z > 0 && z <= ZRES - 1;
}
constexpr bool REVERSE_BOUNDS_CHECK(coord_t x, coord_t y, coord_t z) {
    return x < 1 || x >= XRES - 1 || y < 1 || y >= YRES - 1 || z < 1 || z >= ZRES - 1;
}


#endif