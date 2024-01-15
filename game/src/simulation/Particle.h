#ifndef PARTICLE_H
#define PARTICLE_H

#include "SimulationDef.h"
#include "../util/color.h"

#include <bitset>

namespace PartFlags {
    constexpr uint8_t UPDATE_FRAME = 0;
    constexpr uint8_t MOVE_FRAME = 1;
    constexpr uint8_t IS_ENERGY = 2;
}

struct alignas(64) Particle {
    uint16_t type;
    part_id id;
    std::bitset<8> flag;

    uint16_t ctype;
    int16_t life = 0;
    float x, y, z, vx, vy, vz;
    coord_t rx, ry, rz; // Rounded coordinates
    float temp;
    uint16_t tmp1, tmp2;
    RGBA dcolor{0, 0, 0, 0};

    Particle(): type(0), id(0) {}
    Particle(const Particle&) = delete;
	Particle& operator=(const Particle&) = delete;
};

#endif