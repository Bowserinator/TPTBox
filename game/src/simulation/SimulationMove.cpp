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
bool Simulation::raycast(const RaycastInput &in,  RaycastOutput &out, auto pmapOccupied) const {
    const Vector3T<signed_coord_t> last_voxel{
        (signed_coord_t)((signed_coord_t)in.x + util::ceil_proper(in.vx)),
        (signed_coord_t)((signed_coord_t)in.y + util::ceil_proper(in.vy)),
        (signed_coord_t)((signed_coord_t)in.z + util::ceil_proper(in.vz))
    };
    Vector3T<signed_coord_t> current_voxel{ (signed_coord_t)in.x, (signed_coord_t)in.y, (signed_coord_t)in.z };
    Vector3T<signed_coord_t> diff{ 0, 0, 0 };
    Vector3T<signed_coord_t> previous_voxel = current_voxel;

    const Vector3T<signed_coord_t> ray = last_voxel - current_voxel;

    // Step to take per direction (+-1)
    float dx = (ray.x >= 0) * 2 - 1;
    float dy = (ray.y >= 0) * 2 - 1;
    float dz = (ray.z >= 0) * 2 - 1;

    const Vector3 next_voxel_boundary{ in.x + dx, in.y + dy, in.z + dz };

    // tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
    // the value of t at which the ray crosses the first vertical voxel boundary
    float tMaxX = (ray.x != 0) ? (next_voxel_boundary.x - in.x) / ray.x : std::numeric_limits<float>::max();
    float tMaxY = (ray.y != 0) ? (next_voxel_boundary.y - in.y) / ray.y : std::numeric_limits<float>::max();
    float tMaxZ = (ray.z != 0) ? (next_voxel_boundary.z - in.z) / ray.z : std::numeric_limits<float>::max();

    // tDeltaX, tDeltaY, tDeltaZ --
    // how far along the ray we must move for the horizontal component to equal the width of a voxel
    // the direction in which we traverse the grid
    // can only be FLT_MAX if we never go in that direction
    float tDeltaX = (ray.x != 0) ? 1.0f / ray.x * dx : std::numeric_limits<float>::max();
    float tDeltaY = (ray.y != 0) ? 1.0f / ray.y * dy : std::numeric_limits<float>::max();
    float tDeltaZ = (ray.z != 0) ? 1.0f / ray.z * dz : std::numeric_limits<float>::max();

    bool neg_ray = false;
    if (ray.x < 0 && current_voxel.x != last_voxel.x) { diff.x--; neg_ray = true; }
    if (ray.y < 0 && current_voxel.y != last_voxel.y) { diff.y--; neg_ray = true; }
    if (ray.z < 0 && current_voxel.z != last_voxel.z) { diff.z--; neg_ray = true; }

    // Get which "faces" to bounce off of
    // prev is now, final is the voxel we will collide with if we continue
    // down our current trajectory
    // Precondition: prev_loc != final_loc
    auto getFaces = [this, &pmapOccupied](const Vector3T<signed_coord_t> &prev_loc, const Vector3T<signed_coord_t> &final_loc) -> RayCast::RayHitFace {
        RayCast::RayHitFace faces = 0;

        if ((prev_loc.x != final_loc.x) + (prev_loc.y != final_loc.y) + (prev_loc.z != final_loc.z) == 1) {
            if (prev_loc.x != final_loc.x)
                faces |= RayCast::FACE_X;
            if (prev_loc.y != final_loc.y)
                faces |= RayCast::FACE_Y;
            if (prev_loc.z != final_loc.z)
                faces |= RayCast::FACE_Z;
        } else {
            if (pmapOccupied(Vector3T<signed_coord_t>{ final_loc.x, prev_loc.y, prev_loc.z }))
                faces |= RayCast::FACE_X;
            if (pmapOccupied(Vector3T<signed_coord_t>{ prev_loc.x, final_loc.y, prev_loc.z }))
                faces |= RayCast::FACE_Y;
            if (pmapOccupied(Vector3T<signed_coord_t>{ prev_loc.x, prev_loc.y, final_loc.z }))
                faces |= RayCast::FACE_Z;
        }
        return faces;
    };

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
            out.x = previous_voxel.x;
            out.y = previous_voxel.y;
            out.z = previous_voxel.z;
            if (in.compute_faces)
                out.faces = getFaces(previous_voxel, current_voxel);
            return true;
        }
    }

    out.x = current_voxel.x;
    out.y = current_voxel.y;
    out.z = current_voxel.z;
    out.faces = 0; // No faces to bounce off
    return false;
}


/**
 * @brief Perform default movement behaviors for different
 *        states of matter (ie powder, fluid, etc...)
 * 
 * @param idx 
 */
void Simulation::move_behavior(const int idx) {
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
        auto behavior = eval_move(idx, x, y - 1, z);

        if ((y > 1 && !pmap[z][y - 1][x]) || (y > 1 && behavior != PartSwapBehavior::NOOP)) {
            try_move(idx, x, y - 1, z, behavior);
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
void Simulation::try_move(const int idx, const float tx, const float ty, const float tz, PartSwapBehavior behavior) {
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

    auto &part_map = GetElements()[parts[idx].type].State == ElementState::TYPE_ENERGY ?
        photons : pmap;
    auto old_pmap_val = part_map[oldz][oldy][oldx];

    if (behavior == PartSwapBehavior::NOT_EVALED_YET)
        behavior = eval_move(idx, x, y, z);

    switch (behavior) {
        case PartSwapBehavior::NOOP:
            return;
        case PartSwapBehavior::SWAP:
            swap_part(x, y, z, oldx, oldy, oldz, ID(part_map[z][y][x]), idx);
            break;
        case PartSwapBehavior::OCCUPY_SAME:
            part_map[oldz][oldy][oldx] = 0;
            part_map[z][y][x] = old_pmap_val;
            break;
        // The special behavior is resolved into one of the three
        // cases above by eval_move
    }

    parts[idx].x = tx;
    parts[idx].y = ty;
    parts[idx].z = tz;
}

/**
 * @brief Spatially swap the particles at the two ids
 */
void Simulation::swap_part(const coord_t x1, const coord_t y1, const coord_t z1,
        const coord_t x2, const coord_t y2, const coord_t z2, const int id1, const int id2) {
    std::swap(parts[id1].x, parts[id2].x);
    std::swap(parts[id1].y, parts[id2].y);
    std::swap(parts[id1].z, parts[id2].z);

    // TODO: temp hack
    // also need to consider solid -> energy and vice versa
    auto s1 = GetElements()[parts[id1].type].State;
    auto s2 = GetElements()[parts[id2].type].State;

    if (s1 == ElementState::TYPE_ENERGY || s2 == ElementState::TYPE_ENERGY)
        std::swap(photons[z1][y1][x1], photons[z2][y2][x2]);
    else
        std::swap(pmap[z1][y1][x1], pmap[z2][y2][x2]);
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

    // Condition for early termination:
    // return true if it "hit" something (current spot is occupied or the next spot)
    // is outside of the simulation bounds
    auto pmapOccupied = [idx, this](const Vector3T<signed_coord_t> &loc) -> bool {
        if (REVERSE_BOUNDS_CHECK(loc.x, loc.y, loc.z))
            return true;
        return eval_move(idx, loc.z, loc.y, loc.z) == PartSwapBehavior::NOOP;
    };

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
        }, out, pmapOccupied);

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
        if (fabsf(part.vy) < 0.5f && fabsf(part.vx) < 0.5f && fabsf(part.vx) < 0.5f) {
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

/**
 * @brief Evaluate the behavior when particle moves to new space
 * 
 * @param idx Particle id to move
 * @param nx New loc
 * @param ny New loc
 * @param nz New loc
 * @return part swap behavior, special cases are resolved
 */
PartSwapBehavior Simulation::eval_move(const int idx, const coord_t nx, const coord_t ny, const coord_t nz) const {
    auto other_type = TYP(pmap[nz][ny][nx]);
    if (!other_type) other_type = TYP(photons[nz][ny][nx]);
    if (!other_type) return PartSwapBehavior::SWAP;

    auto this_type = parts[idx].type;

    if (can_move[this_type][other_type] != PartSwapBehavior::SPECIAL)
        return can_move[this_type][other_type];

    // Deal with special cases:
    return PartSwapBehavior::NOOP; // TODO
}
