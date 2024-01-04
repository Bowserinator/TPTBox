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
}


int Simulation::create_part(coord_t x, coord_t y, coord_t z, ElementType type) {
    #ifdef DEBUG
    if (REVERSE_BOUNDS_CHECK(x, y, z))
        throw std::invalid_argument("Input to sim.create_part must be in bounds, got " +
            std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z));
    #endif

    if (pmap[z][y][x]) return -1; // TODO


    // Create new part
    // Note: should it allow creation off screen? 
    // TODO: return element id generated
    parts[pfree].id = pfree;
    parts[pfree].type = type;
    parts[pfree].x = x;
    parts[pfree].y = y;
    parts[pfree].z = z;
    parts[pfree].vx = 0.0f;
    parts[pfree].vy = 0.0f;
    parts[pfree].vz = 0.0f;

    pmap[z][y][x] = pfree;
    pfree++;
    maxId = std::max(maxId, pfree);
    return pfree - 1;
}

void Simulation::kill_part(int i) {
    auto &part = parts[i];

    coord_t x = util::roundf(part.x);
    coord_t y = util::roundf(part.y);
    coord_t z = util::roundf(part.z);
    pmap[z][y][x] = 0;
    part.type = PT_NONE;
    part.id = 0;
}

void Simulation::update() {
    for (int i = 0; i <= maxId; i++)
    {
        auto &part = parts[i];
        if (!part.type) continue; // TODO: can probably be more efficient

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


        if (part.vx || part.vy || part.vz) {
            // Clamp velocity
            part.vx = util::clampf(part.vx, -MAX_VELOCITY, MAX_VELOCITY);
            part.vy = util::clampf(part.vy, -MAX_VELOCITY, MAX_VELOCITY);
            part.vz = util::clampf(part.vz, -MAX_VELOCITY, MAX_VELOCITY);

            coord_t tx, ty, tz;
            bool hit = raycast(x, y, z, std::ceil(part.vx), std::ceil(part.vy), std::ceil(part.vz), tx, ty, tz);

            float ox, oy, oz;
            if (!hit || (tx == x && ty == y && tz == z)) {
                ox = part.x + part.vx;
                oy = part.y + part.vy;
                oz = part.z + part.vz;
            } else {
                ox = tx;
                oy = ty;
                oz = tz;

                auto bounce = GetElements()[part.type].Collision;
                part.vx *= bounce;
                part.vy *= bounce;
                part.vz *= bounce;
            }
            try_move(i, ox, oy, oz);
        }

        move_behavior(i);

    }

    frame_count++;
}

