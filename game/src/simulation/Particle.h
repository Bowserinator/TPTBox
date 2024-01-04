#ifndef PARTICLE_H
#define PARTICLE_H

#include "SimulationDef.h"
#include "../util/color.h"

struct Particle {
    uint16_t type;
    unsigned int id;
    uint16_t ctype;
    int16_t life;
    float x, y, z, vx, vy, vz;
    float temp;
    uint16_t tmp1, tmp2;
    RGBA dcolor;
};

#endif