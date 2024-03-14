#ifndef SIMULATION_H
#define SIMULATION_H

#include "Particle.h"
#include "SimulationDef.h"
#include "SimulationGraphics.h"
#include "SimulationHeat.h"
#include "Gol.h"
#include "Raycast.h"
#include "Air.h"

#include "../util/types/rand.h"
#include "../util/types/heap_array.h"
#include "../util/types/spinlock.h"

#include "../util/math.h"
#include "../util/vector_op.h"
#include "../render/types/octree.h"

#include <omp.h>
#include <vector>

enum class GravityMode {
    VERTICAL = 0,
    ZERO_G = 1,
    RADIAL = 2
};

class Simulation {
public:
    bool paused;
    GravityMode gravity_mode;

    Particle parts[NPARTS];
    pmap_id pmap[ZRES][YRES][XRES];
    pmap_id photons[ZRES][YRES][XRES];
    PartSwapBehavior can_move[ELEMENT_COUNT + 1][ELEMENT_COUNT + 1];
    util::Spinlock parts_add_remove_lock;

    Air air;
    SimulationGol gol;
    SimulationHeat heat;

    part_id pfree;
    part_id maxId;
    
    uint32_t parts_count;
    uint32_t frame_count; // Monotomic frame counter, will overflow in ~824 days @ 60 FPS. Do not keep the program open for this long

    // Graphics bookkeeping
    SimulationGraphics graphics;

    unsigned int sim_thread_count;
    unsigned int actual_thread_count;
    unsigned int max_ok_causality_range;
    coord_t min_y_per_zslice[ZRES - 2];
    coord_t max_y_per_zslice[ZRES - 2];
    std::vector<RNG> rngs;


    Simulation();
    ~Simulation();

    void init();
    void cycle_gravity_mode();
    void set_paused(const bool paused) { this->paused = paused; };

    part_id create_part(const coord_t x, const coord_t y, const coord_t z, const ElementType type);
    void kill_part(const part_id id);
    bool part_change_type(const part_id i, const part_type new_type);

    void update();
    void dispatch_compute_shaders();
    void download_heat_from_gpu();
    void update_zslice(const coord_t zslice);
    void recalc_free_particles();

    void update_part(const part_id i, const bool consider_causality = true);

    void move_behavior(const part_id idx);
    void try_move(const part_id idx, const float x, const float y, const float z,
        PartSwapBehavior behavior = PartSwapBehavior::NOT_EVALED_YET);
    void swap_part(const coord_t x1, const coord_t y1, const coord_t z1,
        const coord_t x2, const coord_t y2, const coord_t z2,
        const part_id id1, const part_id id2);

    inline RNG& rng() { return rngs[omp_get_thread_num()]; }

    // Whether to figure out which faces collided with (true)
    // whether to return space before collision (false) or the position of the particle ray collided with (true)
    template <bool compute_faces, bool take_intersect>
    bool raycast(const RaycastInput &in, RaycastOutput &out, const auto pmapOccupied) const;

    PartSwapBehavior eval_move(const part_id idx, const coord_t nx, const coord_t ny, const coord_t nz) const;

    static const char * getGravityModeName(const GravityMode mode) {
        switch (mode) {
            case GravityMode::VERTICAL:
                return "Vertical";
            case GravityMode::ZERO_G:
                return "Off";
            case GravityMode::RADIAL:
                return "Radial";
        }
        return "Unknown";
    }
private:
    void _init_can_move();
    void _raycast_movement(const part_id idx, const coord_t x, const coord_t y, const coord_t z);
    void _set_color_data_at(const coord_t x, const coord_t y, const coord_t z, const Particle * part);
    void _update_shadow_map(const coord_t x, const coord_t y, const coord_t z);
    bool _should_do_lighting(const Particle &part);
    void _force_update_all_shadows();
};



/**
 * @brief Perform a raycast starting at (x,y,z) with max displacement
 *        and direction indicated by (vx, vy, vz). The last empty voxel
 *        before colliding is written to (ox, oy, oz)
 * 
 *        Pmap occupied is function that takes in Vector3T<coord_t> and returns
 *        PartSwapBehavior
 * @see https://github.com/francisengelmann/fast_voxel_traversal/tree/master
 *        Licensed under LICENSES/
 * @tparam compute_faces If true will output .faces for XYZ faces collided with
 * @tparam take_intersect If true output pos is the position of the particle ray collided with
 *                        If false output pos is the space right before colliding
 * @return Whether it terminated because it hit a voxel (true if yes)
 */
template <bool compute_faces = false, bool take_intersect = false>
bool Simulation::raycast(const RaycastInput &in, RaycastOutput &out, const auto pmapOccupied) const {
    // For raycasts that stop early, ie right on the next frame, we can simply check
    // if there's a particle in the direction of the greatest velocity. This saves
    // ~5ms per frame on a grid of 350k water particles
    // When the grid is that full, statistically most particles will not be able to move
    // hence the "optimization"
    int largest_axis = util::argmax3(in.vx, in.vy, in.vz);
    bool early_stop = false;

    if (largest_axis == 0 && PartSwapBehavior::NOOP == pmapOccupied(Vector3T<signed_coord_t>{ (signed_coord_t)(in.x + (in.vx < 0 ? -1 : 1)), (signed_coord_t)in.y, (signed_coord_t)in.z })) {
        early_stop = true;
        if (compute_faces)
            out.faces = RayCast::FACE_X;
    }
    else if (largest_axis == 1 && PartSwapBehavior::NOOP == pmapOccupied(Vector3T<signed_coord_t>{ (signed_coord_t)in.x, (signed_coord_t)(in.y + (in.vy < 0 ? -1 : 1)), (signed_coord_t)in.z })) {
        early_stop = true;
        if (compute_faces)
            out.faces = RayCast::FACE_Y;
    }
    else if (largest_axis == 2 && PartSwapBehavior::NOOP == pmapOccupied(Vector3T<signed_coord_t>{ (signed_coord_t)in.x, (signed_coord_t)in.y, (signed_coord_t)(in.z + (in.vz < 0 ? -1 : 1)) })) {
        early_stop = true;
        if (compute_faces)
            out.faces = RayCast::FACE_Z;
    }
    if (early_stop) {
        out.x = in.x;
        out.y = in.y;
        out.z = in.z;
        out.move = PartSwapBehavior::NOOP;
        return true;
    }

    // Actual raycast --------------
    Vector3T<signed_coord_t> current_voxel{ (signed_coord_t)in.x, (signed_coord_t)in.y, (signed_coord_t)in.z };
    const Vector3T<signed_coord_t> last_voxel{
        (signed_coord_t)((signed_coord_t)in.x + util::ceil_proper(in.vx)),
        (signed_coord_t)((signed_coord_t)in.y + util::ceil_proper(in.vy)),
        (signed_coord_t)((signed_coord_t)in.z + util::ceil_proper(in.vz))
    };
    Vector3T<signed_coord_t> diff{ 0, 0, 0 };
    Vector3T<signed_coord_t> previous_voxel = current_voxel;

    const Vector3T<signed_coord_t> ray = last_voxel - current_voxel;

    // Step to take per direction (+-1)
    const float dx = (ray.x >= 0) ? 1 : -1;
    const float dy = (ray.y >= 0) ? 1 : -1;
    const float dz = (ray.z >= 0) ? 1 : -1;

    const Vector3 next_voxel_boundary{ in.x + dx, in.y + dy, in.z + dz };

    // tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
    // the value of t at which the ray crosses the first vertical voxel boundary
    float tMaxX = (ray.x != 0) ? (dx) / ray.x : std::numeric_limits<float>::max();
    float tMaxY = (ray.y != 0) ? (dy) / ray.y : std::numeric_limits<float>::max();
    float tMaxZ = (ray.z != 0) ? (dz) / ray.z : std::numeric_limits<float>::max();

    // tDeltaX, tDeltaY, tDeltaZ --
    // how far along the ray we must move for the horizontal component to equal the width of a voxel
    // the direction in which we traverse the grid
    // can only be FLT_MAX if we never go in that direction
    const float tDeltaX = (ray.x != 0) ? 1.0f / ray.x * dx : std::numeric_limits<float>::max();
    const float tDeltaY = (ray.y != 0) ? 1.0f / ray.y * dy : std::numeric_limits<float>::max();
    const float tDeltaZ = (ray.z != 0) ? 1.0f / ray.z * dz : std::numeric_limits<float>::max();

    if (ray.x < 0 && current_voxel.x != last_voxel.x) { diff.x--; }
    if (ray.y < 0 && current_voxel.y != last_voxel.y) { diff.y--; }
    if (ray.z < 0 && current_voxel.z != last_voxel.z) { diff.z--; }

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
            if (PartSwapBehavior::NOOP == pmapOccupied(Vector3T<signed_coord_t>{ final_loc.x, prev_loc.y, prev_loc.z }))
                faces |= RayCast::FACE_X;
            if (PartSwapBehavior::NOOP == pmapOccupied(Vector3T<signed_coord_t>{ prev_loc.x, final_loc.y, prev_loc.z }))
                faces |= RayCast::FACE_Y;
            if (PartSwapBehavior::NOOP == pmapOccupied(Vector3T<signed_coord_t>{ prev_loc.x, prev_loc.y, final_loc.z }))
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

        if (PartSwapBehavior::NOOP == pmapOccupied(current_voxel)) {
            auto voxel = take_intersect ? current_voxel : previous_voxel;
            out.x = voxel.x;
            out.y = voxel.y;
            out.z = voxel.z;
            out.move = PartSwapBehavior::SWAP;

            if (compute_faces)
                out.faces = getFaces(previous_voxel, current_voxel);
            return true;
        }
    }

    out.x = current_voxel.x;
    out.y = current_voxel.y;
    out.z = current_voxel.z;
    if (compute_faces)
        out.faces = 0; // No faces to bounce off
    out.move = PartSwapBehavior::SWAP;
    return false;
}

#endif