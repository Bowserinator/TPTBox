#include "Simulation.h"
#include "ElementClasses.h"
#include "ElementDefs.h"
#include "../util/vector_op.h"
#include "../util/util.h"

#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>
#include <cmath>
#include <limits>

Simulation::Simulation():
    paused(false)
{
    std::fill(&pmap[0][0][0], &pmap[ZRES][YRES][XRES], 0);
    // std::fill(&parts[0], &parts[NPARTS], 0);

    pfree = 1;
    maxId = 0;
    frame_count = 0;
    parts_count = 0;
}


int Simulation::create_part(const coord_t x, const coord_t y, const coord_t z, const ElementType type) {
    #ifdef DEBUG
    if (REVERSE_BOUNDS_CHECK(x, y, z))
        throw std::invalid_argument("Input to sim.create_part must be in bounds, got " +
            std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z));
    #endif

    if (pmap[z][y][x]) return -1; // TODO
    if (pfree >= NPARTS) return -3; // TODO

    // Create new part
    // Note: should it allow creation off screen? 
    // TODO: return element id generated
    int next_pfree = parts[pfree].id < 0 ? -parts[pfree].id : pfree + 1;
    int old_pfree = pfree;

    parts[pfree].id = pfree;
    parts[pfree].type = type;
    parts[pfree].x = x;
    parts[pfree].y = y;
    parts[pfree].z = z;
    parts[pfree].vx = 0.0f;
    parts[pfree].vy = 0.0f;
    parts[pfree].vz = 0.0f;

    pmap[z][y][x] = pfree;
    maxId = std::max(maxId, pfree + 1);
    pfree = next_pfree;
    return old_pfree;
}

void Simulation::kill_part(const int i) {
    auto &part = parts[i];
    if (part.type <= 0) return;

    coord_t x = util::roundf(part.x);
    coord_t y = util::roundf(part.y);
    coord_t z = util::roundf(part.z);
    pmap[z][y][x] = 0;
    part.type = PT_NONE;

    if (i == maxId && i > 0)
        maxId--;

    part.id = -pfree;
    pfree = i;
}

void Simulation::update() {
    parts_count = 0;
    int newMaxId = 0;

    for (int i = 0; i <= maxId; i++)
    {
        auto &part = parts[i];
        if (!part.type) continue; // TODO: can probably be more efficient

        parts_count++;
        newMaxId = i;

        // TODO: tmp hack for spherical gravity
        float dx = XRES / 2 - part.x;
        float dy = YRES / 2 - part.y;
        float dz = ZRES / 2 - part.z;
        float dis = std::sqrt(dx *dx+dy*dy+dz*dz);
        dx /= dis;
        dy /= dis;
        dz /= dis;
        float F = 5.0f;
        //part.vx = F *dx;
        //part.vy = F* dy;
       // part.vz = F*dz;

        coord_t x = util::roundf(part.x);
        coord_t y = util::roundf(part.y);
        coord_t z = util::roundf(part.z);

        if (GetElements()[part.type].Update) {
            auto result = GetElements()[part.type].Update(this, i, x, y, z, parts, pmap);
            if (result == -1) continue; // TODO: flags or something
        }

        if (part.vx || part.vy || part.vz)
            _raycast_movement(i, x, y, z); // Apply velocity to displacement
        move_behavior(i); // Apply element specific movement, like powder / liquid spread

    }
    maxId = newMaxId + 1;

    frame_count++;
}

// Try to move a particle with velocity to new location
void Simulation::_raycast_movement(const int idx, const coord_t x, const coord_t y, const coord_t z) {
    auto &part = parts[idx];
    part.vx = util::clampf(part.vx, -MAX_VELOCITY, MAX_VELOCITY);
    part.vy = util::clampf(part.vy, -MAX_VELOCITY, MAX_VELOCITY);
    part.vz = util::clampf(part.vz, -MAX_VELOCITY, MAX_VELOCITY);

    RaycastOutput out;
    coord_t sx = x; // Starting point of raycast, can change with repeated casts
    coord_t sy = y;
    coord_t sz = z;
    bool hit, no_move; // Whether we hit an edge or voxel, whether we moved on the current raycast

    // Repeatedly ray cast until we "run out" of distance
    // Initial distance being the magnitude of the velocity vector
    float portion_velocity = 1.0f;
    float org_dis = util::hypot(part.vx, part.vy, part.vz);

    do {
        hit = raycast(RaycastInput {
            .x = sx, .y = sy, .z = sz,
            .vx = part.vx * portion_velocity,
            .vy = part.vy * portion_velocity,
            .vz = part.vz * portion_velocity,
            .compute_faces = true
        }, out);

        // We add a small offset since voxels are always 1.0f apart, so we add a small bit to prevent
        // rounding error (optimistically over-consuming distance to avoid extra unnecessary rays)
        portion_velocity -= util::hypot(out.x - sx, out.y - sy, out.z - sz) / org_dis + 0.001f;

        no_move = out.x == sx && out.y == sy && out.z == sz;
        sx = out.x; sy = out.y; sz = out.z;

        auto bounce = GetElements()[part.type].Collision;
        if ((out.faces & RayCast::FACE_X).any()) part.vx *= bounce;
        if ((out.faces & RayCast::FACE_Y).any()) part.vy *= bounce;
        if ((out.faces & RayCast::FACE_Z).any()) part.vz *= bounce;

        // The little velocity we have left is not enough to move to
        // another voxel, so the next raycast is always no_move, terminate early
        if (fabsf(part.vx) < 0.5f && fabsf(part.vy) < 0.5f && fabsf(part.vx) < 0.5f) {
            no_move = true;
            break;
        }
    } while(!(hit || no_move || portion_velocity <= 0.5f));

    float ox, oy, oz;
    if (!hit || no_move) {
        ox = util::clampf(part.x + part.vx, 1.0f, XRES - 1.0f);
        oy = util::clampf(part.y + part.vy, 1.0f, YRES - 1.0f);
        oz = util::clampf(part.z + part.vz, 1.0f, ZRES - 1.0f);
    } else {
        ox = out.x;
        oy = out.y;
        oz = out.z;
    }

    try_move(idx, ox, oy, oz);
}
