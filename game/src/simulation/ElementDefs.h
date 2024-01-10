#ifndef ELEMENT_DEFS_H
#define ELEMENT_DEFS_H

#include "stdint.h"
#include "Simulation.h"

enum class ElementState : uint8_t { TYPE_SOLID, TYPE_POWDER, TYPE_LIQUID, TYPE_GAS, TYPE_ENERGY };

constexpr float UNSET_PROPERTY = -9996.0f; // Honestly just a random value, pls don't use this

#define UPDATE_FUNC_ARGS Simulation* sim, int i, coord_t x, coord_t y, coord_t z, Particle *parts, pmap_id pmap[ZRES][YRES][XRES]

#endif