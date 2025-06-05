#ifndef SIMULATION_PARTICLE_H_
#define SIMULATION_PARTICLE_H_

#include "SimulationDef.h"
#include "graphics/color.h"
#include "util/types/bitset8.h"

namespace PartFlags {
    constexpr uint8_t UPDATE_FRAME = 0;
    constexpr uint8_t MOVE_FRAME = 1;
    constexpr uint8_t IS_ENERGY = 2;
}

struct DefaultParticleProperties {
    float temp;
    part_type ctype;
    int16_t life = 0;
    uint16_t tmp1, tmp2;
    RGBA dcolor{0, 0, 0, 0};
};

struct Particle  {
    part_id id;
    float x, y, z, vx, vy, vz;
    coord_t rx, ry, rz; // Rounded coordinates
    RGBA dcolor{0, 0, 0, 0};

    part_type type;
    util::Bitset8 flag;

    int16_t life = 0;
    part_type ctype;
    uint16_t tmp1, tmp2;

    Particle(): id(0), type(0) {}
};

#endif // SIMULATION_PARTICLE_H_
