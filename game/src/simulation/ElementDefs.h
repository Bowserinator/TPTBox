#ifndef ELEMENT_DEFS_H
#define ELEMENT_DEFS_H

#include "stdint.h"
#include "../util/types/color.h"
#include "../util/types/bitset8.h"
#include "Simulation.h"

enum class ElementState : uint8_t { TYPE_SOLID, TYPE_POWDER, TYPE_LIQUID, TYPE_GAS, TYPE_ENERGY };

#define UPDATE_FUNC_ARGS Simulation &sim, int i, coord_t x, coord_t y, coord_t z, Particle *parts, pmap_id pmap[ZRES][YRES][XRES]
#define GRAPHICS_FUNC_ARGS Simulation &sim, const Particle &part, coord_t x, coord_t y, coord_t z, RGBA &color, util::Bitset8 &flags

#endif