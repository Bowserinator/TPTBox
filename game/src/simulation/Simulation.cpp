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
    
    // TODO: singleton?
    _init_can_move();
}

void Simulation::_init_can_move() {
    ElementType movingType, destinationType;
    const auto &elements = GetElements();

    for (movingType = 1; movingType < ELEMENT_COUNT; movingType++) {
		for (destinationType = 1; destinationType < ELEMENT_COUNT; destinationType++) {
            // Heavier elements can swap with lighter ones
			if (elements[movingType].Weight > elements[destinationType].Weight)
				can_move[movingType][destinationType] = PartSwapBehavior::SWAP;
        }
    }
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
        // float dx = XRES / 2 - part.x;
        // float dy = YRES / 2 - part.y;
        // float dz = ZRES / 2 - part.z;
        // float dis = std::sqrt(dx *dx+dy*dy+dz*dz);
        // dx /= dis;
        // dy /= dis;
        // dz /= dis;
        // float F = 100.0f / std::min(10000.0f, dis * dis);
        // part.vx += F *dx;
        // part.vy += F* dy;
        // part.vz += F*dz;

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
