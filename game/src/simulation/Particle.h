#ifndef PARTICLE_H
#define PARTICLE_H

#include "SimulationDef.h"
#include "../util/color.h"

#include <bitset>

namespace PartFlags {
    constexpr uint8_t UPDATE_FRAME = 0;
}

struct Particle {
    uint16_t type;
    part_id id;
    uint16_t ctype;
    int16_t life = 0;
    float x, y, z, vx, vy, vz;
    float temp;
    uint16_t tmp1, tmp2;
    RGBA dcolor;
    std::bitset<8> flag;

    Particle() {}
    Particle(const Particle&) = delete;
	Particle& operator=(const Particle&) = delete;
};

#endif