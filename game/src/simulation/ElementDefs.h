#ifndef ELEMENT_DEFS_H
#define ELEMENT_DEFS_H

#include "stdint.h"
#include "Simulation.h"

constexpr unsigned short PT_NUM = 1024; // TODO should be max possible

enum class ElementState : uint8_t { TYPE_SOLID, TYPE_POWDER, TYPE_LIQUID, TYPE_GAS, TYPE_ENERGY };

#define UPDATE_FUNC_ARGS Simulation* sim, int i, coord_t x, coord_t y, coord_t z, Particle *parts, unsigned int pmap[ZRES][YRES][XRES]

#endif