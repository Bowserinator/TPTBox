#ifndef PARTICLE_H
#define PARTICLE_H

struct Particle {
    unsigned int type;
    unsigned int id;
    unsigned int life, ctype;
    float x, y, z, vx, vy, vz;
    float temp;
    int tmp1, tmp2, tmp3, tmp4, flags;
    unsigned int dcolor;
};

#endif