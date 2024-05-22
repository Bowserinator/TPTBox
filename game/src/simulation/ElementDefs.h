#ifndef ELEMENT_DEFS_H
#define ELEMENT_DEFS_H

#include "stdint.h"
#include "../graphics/color.h"
#include "../util/types/bitset8.h"
#include "Simulation.h"

enum class ElementState : uint8_t { TYPE_SOLID, TYPE_POWDER, TYPE_LIQUID, TYPE_GAS, TYPE_ENERGY };

namespace ElementProperties {
    constexpr uint32_t LIFE_DEC  = 0x00000001; // 2^0 Life decrements to 0 every frame
    constexpr uint32_t LIFE_KILL = 0x00000002; // 2^1 Killed when life value <= 0

    // Shorthands for multiple properties
    constexpr uint32_t LIFE_DEC_KILL = LIFE_DEC | LIFE_KILL;
}

#define UPDATE_FUNC_ARGS Simulation &sim, int i, coord_t x, coord_t y, coord_t z, Particle *parts, pmap_id pmap[ZRES][YRES][XRES]
#define GRAPHICS_FUNC_ARGS Simulation &sim, const Particle &part, coord_t x, coord_t y, coord_t z, RGBA &color, util::Bitset8 &flags
#define CREATE_ALLOWED_FUNC_ARGS Simulation &sim, part_id i, coord_t x, coord_t y, coord_t z, part_type type
#define ON_CREATE_FUNC_ARGS Simulation &sim, part_id i, coord_t x, coord_t y, coord_t z, part_type type, const PartCreateMode mode
#define ON_CHANGE_TYPE_FUNC_ARGS Simulation &sim, part_id i, coord_t x, coord_t y, coord_t z, part_type oldType, part_type newType

#endif