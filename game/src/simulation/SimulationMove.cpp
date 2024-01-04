#include "Simulation.h"
#include "ElementClasses.h"
#include "ElementDefs.h"
#include "../util/vector_op.h"
#include "../util/util.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>

/**
 * @brief Perform a raycast starting at (x,y,z) with max displacement
 *        and direction indicated by (vx, vy, vz). The last empty voxel
 *        before colliding is written to (ox, oy, oz)
 * @see https://github.com/francisengelmann/fast_voxel_traversal/tree/master
 *        Licensed under LICENSES/
 * @return Whether it terminated because it hit a voxel (true if yes)
 */
bool Simulation::raycast(coord_t x, coord_t y, coord_t z, float vx, float vy, float vz, coord_t &ox, coord_t &oy, coord_t &oz) const {
    // Step to take per direction (+-1)
    float dx = (vx >= 0) * 2 - 1;
    float dy = (vy >= 0) * 2 - 1;
    float dz = (vz >= 0) * 2 - 1;

    const Vector3T<short> last_voxel{ (short)(x + std::floor(vx)), (short)(y + std::floor(vy)), (short)(z + std::floor(vz)) };
    const Vector3 next_voxel_boundary{ x + dx, y + dy, z + dz };

    // tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
    // the value of t at which the ray crosses the first vertical voxel boundary
    float tMaxX = (vx != 0) ? (next_voxel_boundary.x - x) / vx : std::numeric_limits<float>::max();
    float tMaxY = (vy != 0) ? (next_voxel_boundary.y - y) / vy : std::numeric_limits<float>::max();
    float tMaxZ = (vz != 0) ? (next_voxel_boundary.z - z) / vz : std::numeric_limits<float>::max();

    // tDeltaX, tDeltaY, tDeltaZ --
    // how far along the ray we must move for the horizontal component to equal the width of a voxel
    // the direction in which we traverse the grid
    // can only be FLT_MAX if we never go in that direction
    float tDeltaX = (vx != 0) ? 1.0f / vx * dx : std::numeric_limits<float>::max();
    float tDeltaY = (vy != 0) ? 1.0f / vy * dy : std::numeric_limits<float>::max();
    float tDeltaZ = (vz != 0) ? 1.0f / vz * dz : std::numeric_limits<float>::max();

    Vector3T<short> current_voxel{ (short)x, (short)y, (short)z };
    Vector3T<short> diff{ 0, 0, 0 };

    bool neg_ray = false;
    if (vx < 0 && current_voxel.x != last_voxel.x) { diff.x--; neg_ray = true; }
    if (vy < 0 && current_voxel.y != last_voxel.y) { diff.y--; neg_ray = true; }
    if (vz < 0 && current_voxel.z != last_voxel.z) { diff.z--; neg_ray = true; }

    // Condition for early termination:
    // return true if it "hit" something (current spot is occupied or the next spot)
    // is outside of the simulation bounds
    auto pmapOccupied = [this](const Vector3T<short> &loc) -> bool {
        if (REVERSE_BOUNDS_CHECK(loc.x, loc.y, loc.z))
            return true;
        return pmap[loc.z][loc.y][loc.x] > 0;
    };

    if (neg_ray) {
        current_voxel += diff;
        if (pmapOccupied(current_voxel)) {
            ox = current_voxel.x;
            oy = current_voxel.y;
            oz = current_voxel.z;
            return true;
        }
    }

    Vector3T<short> previous_voxel = current_voxel;

    while (current_voxel != last_voxel) {
        previous_voxel = current_voxel;

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                current_voxel.x += dx;
                tMaxX += tDeltaX;
            } else {
                current_voxel.z += dz;
                tMaxZ += tDeltaZ;
            }
        } else {
            if (tMaxY < tMaxZ) {
                current_voxel.y += dy;
                tMaxY += tDeltaY;
            } else {
                current_voxel.z += dz;
                tMaxZ += tDeltaZ;
            }
        }
    
        if (pmapOccupied(current_voxel)) {
            ox = previous_voxel.x;
            oy = previous_voxel.y;
            oz = previous_voxel.z;
            return true;
        }
    }
    ox = current_voxel.x;
    oy = current_voxel.y;
    oz = current_voxel.z;
    return false;
}


/**
 * @brief Perform default movement behaviors for different
 *        states of matter (ie powder, fluid, etc...)
 * 
 * @param idx 
 */
void Simulation::move_behavior(int idx) {
    const auto el = GetElements()[parts[idx].type];
    if (el.State == ElementState::TYPE_SOLID) return; // Solids can't move

    const auto part = parts[idx];

    coord_t x = util::roundf(part.x);
    coord_t y = util::roundf(part.y);
    coord_t z = util::roundf(part.z);

    if (el.State == ElementState::TYPE_LIQUID || el.State == ElementState::TYPE_POWDER) {
        // TODO: gravity
        // If nothing below go straight down
        // TODO: temp hack
        if ((y > 1 && !pmap[z][y - 1][x]) || (y > 1 && el.State == ElementState::TYPE_POWDER &&
                GetElements()[parts[pmap[z][y-1][x]].type].State == ElementState::TYPE_LIQUID)) {
            try_move(idx, x, y - 1, z);
            return;
        }

        // Check surroundings or below surroundings
        std::array<int8_t, 2 * 8> next; // 8 neighboring spots it could go
        std::size_t next_spot_count = 0;

        const int ylvl = el.State == ElementState::TYPE_LIQUID ? y : y - 1;
        const float ylvlf = el.State == ElementState::TYPE_LIQUID ? part.y : part.y - 1;

        if (ylvl >= 1 && (pmap[z][y - 1][x] > 0 || y == 1)) {
            for (int dz = -1; dz <= 1; dz++)
            for (int dx = -1; dx <= 1; dx++) {
                if (!dx && !dz) continue;
                if (pmap[z + dz][y][x + dx] == 0 && pmap[z + dz][ylvl][x + dx] == 0) {
                    next[next_spot_count++] = dx;
                    next[next_spot_count++] = dz;
                }
            }
            
            if (next_spot_count) {
                int spot_idx = rand() % (next_spot_count / 2) * 2;
                try_move(idx, part.x + next[spot_idx], ylvlf, part.z + next[spot_idx + 1]);
            }
        }
    }
    else if (el.State == ElementState::TYPE_GAS) {
        std::array<int8_t, 3 * 26> next; // 26 neighboring spots it could go
        std::size_t next_spot_count = 0;

        for (int dz = -1; dz <= 1; dz++)
        for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            if (!dx && !dz && !dy) continue;
            if (pmap[z + dz][y + dy][x + dx] == 0) {
                next[next_spot_count++] = dx;
                next[next_spot_count++] = dy;
                next[next_spot_count++] = dz;
            }
        }
        if (next_spot_count) {
            int spot_idx = (rand() % (next_spot_count / 3)) * 3;
            try_move(idx, part.x + next[spot_idx], part.y + next[spot_idx + 1], part.z + next[spot_idx + 2]);
        }
    }
}

/**
 * @brief Try to move a particle to target location
 *        Updates the pmap and particle properties
 * @param idx Index in parts
 * @param x Target x
 * @param y Target y
 * @param z Target z
 */
void Simulation::try_move(int idx, float tx, float ty, float tz) {
    coord_t x = util::roundf(tx);
    coord_t y = util::roundf(ty);
    coord_t z = util::roundf(tz);

    // TODO: consider edge mode
    if (REVERSE_BOUNDS_CHECK(x, y, z))
        return;
        
    coord_t oldx = util::roundf(parts[idx].x);
    coord_t oldy = util::roundf(parts[idx].y);
    coord_t oldz = util::roundf(parts[idx].z);
    
    // Particle did not move, but target pos could
    // potentially have changed (ie +0.1) but not
    // enough to shift the rounding, so assign anyways
    if (x == oldx && y == oldy && z == oldz) {
        parts[idx].x = tx;
        parts[idx].y = ty;
        parts[idx].z = tz;
        return;
    }

    if (pmap[z][y][x]) {
        // Swap locations?
        // TODO: can move
        swap_part(x, y, z, oldx, oldy, oldz, pmap[z][y][x], idx);
    } else {
        pmap[oldz][oldy][oldx] = 0;
    }

    parts[idx].x = tx;
    parts[idx].y = ty;
    parts[idx].z = tz;
    pmap[z][y][x] = parts[idx].id;
}

/**
 * @brief Spatially swap the particles at the two ids
 */
void Simulation::swap_part(coord_t x1, coord_t y1, coord_t z1, coord_t x2, coord_t y2, coord_t z2, int id1, int id2) {
    std::swap(parts[id1].x, parts[id2].x);
    std::swap(parts[id1].y, parts[id2].y);
    std::swap(parts[id1].z, parts[id2].z);
    std::swap(pmap[z1][y1][x1], pmap[z2][y2][x2]);
}
