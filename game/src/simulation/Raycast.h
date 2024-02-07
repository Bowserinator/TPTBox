#ifndef RAYCAST_H
#define RAYCAST_H

#include "SimulationDef.h"
#include "../util/types/bitset8.h"

namespace RayCast {
    using RayHitFace = util::Bitset8;
    const RayHitFace FACE_X = 0b1;
    const RayHitFace FACE_Y = 0b10;
    const RayHitFace FACE_Z = 0b100;
}

struct RaycastInput {
    coord_t x, y, z;
    float vx, vy, vz;
};

struct RaycastOutput {
    coord_t x, y, z;
    RayCast::RayHitFace faces;
    PartSwapBehavior move;
};

#endif
