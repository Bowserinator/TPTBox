#ifndef PARTICLE_H
#define PARTICLE_H

#include "SimulationDef.h"
#include "../util/types/color.h"
#include "../util/types/bitset8.h"

namespace PartFlags {
    constexpr uint8_t UPDATE_FRAME = 0;
    constexpr uint8_t MOVE_FRAME = 1;
    constexpr uint8_t IS_ENERGY = 2;
}

struct Particle {
    uint16_t type;
    part_id id;
    util::Bitset8 flag;

    uint16_t ctype;
    int16_t life = 0;
    float x, y, z, vx, vy, vz;
    coord_t rx, ry, rz; // Rounded coordinates
    float temp;
    uint16_t tmp1, tmp2;
    float temp_tmp;
    RGBA dcolor{0, 0, 0, 0};

    Particle(): type(0), id(0) {}
    Particle(const Particle&) = delete;
	Particle& operator=(const Particle&) = delete;

    void assign_with_defaults(const Particle &def) {
        ctype = def.ctype;
        life = def.life;
        temp = def.temp;
        temp_tmp = def.temp;
        tmp1 = def.tmp1;
        tmp2 = def.tmp2;
        dcolor = def.dcolor;
    }
};

#endif