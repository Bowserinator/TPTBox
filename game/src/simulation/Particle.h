#ifndef PARTICLE_H
#define PARTICLE_H

#include "SimulationDef.h"
#include "../util/color.h"

struct Particle {
    uint16_t type;
    int id; // Must be signed
    uint16_t ctype;
    int16_t life = 0;
    float x, y, z, vx, vy, vz;
    float temp;
    uint16_t tmp1, tmp2;
    RGBA dcolor;
    uint8_t flag;
};

#endif