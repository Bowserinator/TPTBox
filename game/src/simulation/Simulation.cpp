#include "Simulation.h"
#include "ElementClasses.h"
#include "ElementDefs.h"
#include "../util/vector_op.h"
#include "../util/math.h"

#include <omp.h>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>
#include <cmath>
#include <limits>
#include <cstring>
#include <cstdint>

Simulation::Simulation():
    paused(false),
    air(*this)
{
    memset(&pmap[0][0][0], 0, sizeof(pmap));
    memset(&photons[0][0][0], 0, sizeof(photons));

    std::fill(&max_y_per_zslice[0], &max_y_per_zslice[ZRES - 2], YRES - 1);
    std::fill(&min_y_per_zslice[0], &min_y_per_zslice[ZRES - 2], 1);
    // std::fill(&parts[0], &parts[NPARTS], 0);

    pfree = 1;
    maxId = 0;
    frame_count = 0;
    parts_count = 0;
    gravity_mode = GravityMode::VERTICAL;
    heat.uploadedOnce = false;

    // gravity_mode = GravityMode::RADIAL; // TODO
    

    // ---- Threads ------
    constexpr int MIN_CASUALITY_RADIUS = 4; // Width of each slice = 2 * this
    constexpr int MAX_THREADS = ZRES / (4 * MIN_CASUALITY_RADIUS); // Threads = number of slices / 2

    sim_thread_count = std::min(omp_get_max_threads(), MAX_THREADS);
    max_ok_causality_range = ZRES / (sim_thread_count * 4);
    actual_thread_count = 0;

    for (std::size_t _ = 0; _ < sim_thread_count; _++)
        rngs.push_back(RNG());

    // TODO: singleton?
    _init_can_move();
}

Simulation::~Simulation() {}

// Called when OpenGL context is initialized
void Simulation::init() {
    gol.init();
    heat.init();
}

void Simulation::reset() {
    memset(&pmap[0][0][0], 0, sizeof(pmap));
    memset(&photons[0][0][0], 0, sizeof(photons));

    std::fill(&max_y_per_zslice[0], &max_y_per_zslice[ZRES - 2], YRES - 1);
    std::fill(&min_y_per_zslice[0], &min_y_per_zslice[ZRES - 2], 1);

    for (int i = 1; i < NPARTS; i++)
        if (parts[i].type) kill_part(i);

    pfree = 1;
    maxId = 0;
    frame_count = 0;
    parts_count = 0;
    gravity_mode = GravityMode::VERTICAL;

    graphics.reset();
    heat.reset();
    gol.reset();
}

void Simulation::_init_can_move() {
    ElementType movingType, destinationType;
    const auto &elements = GetElements();

    for (movingType = 1; movingType <= ELEMENT_COUNT; movingType++) {
        // All elements swap with NONE
        can_move[movingType][PT_NONE] = PartSwapBehavior::SWAP;
        can_move[PT_NONE][movingType] = PartSwapBehavior::SWAP;

        // Photons can move through:
        can_move[PT_PHOT][PT_GLAS] = PartSwapBehavior::OCCUPY_SAME;

		for (destinationType = 1; destinationType <= ELEMENT_COUNT; destinationType++) {
            // Heavier elements can swap with lighter ones
			if (elements[movingType].Weight > elements[destinationType].Weight)
				can_move[movingType][destinationType] = PartSwapBehavior::SWAP;

            // All energy particles can occupy same space
            if (elements[movingType].State == ElementState::TYPE_ENERGY && elements[destinationType].State == ElementState::TYPE_ENERGY)
                can_move[movingType][destinationType] = PartSwapBehavior::OCCUPY_SAME;
        }
    }
}

void Simulation::cycle_gravity_mode() {
    gravity_mode = static_cast<GravityMode>( ((int)gravity_mode + 1) % 3 );
}

part_id Simulation::create_part(const coord_t x, const coord_t y, const coord_t z, const ElementType type) {
    #ifdef DEBUG
    if (REVERSE_BOUNDS_CHECK(x, y, z))
        throw std::invalid_argument("Input to sim.create_part must be in bounds, got " +
            std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z));
    #endif

    const auto &el = GetElements()[type];
    const auto is_energy = el.State == ElementState::TYPE_ENERGY;
    const auto part_map = is_energy ? photons : pmap;

    if (part_map[z][y][x]) return PartErr::ALREADY_OCCUPIED;

    // Begin PFREE MODIFICATION
    util::unique_spinlock _lock(parts_add_remove_lock);

    if ((unsigned int)pfree >= NPARTS) return PartErr::PARTS_FULL;
    if (el.CreateAllowed && !el.CreateAllowed(*this, pfree, x, y, z, type))
        return PartErr::NOT_ALLOWED;

    const part_id old_pfree = pfree;
    const part_id next_pfree = parts[old_pfree].id < 0 ? -parts[old_pfree].id : old_pfree + 1;

    maxId = std::max(maxId, old_pfree + 1);
    pfree = next_pfree;
    _lock.unlock();
    // END PFREE MODIFICATION, we now have access to old_pfree

    parts[old_pfree].flag[PartFlags::UPDATE_FRAME] = 1 - (frame_count & 1);
    parts[old_pfree].flag[PartFlags::MOVE_FRAME]   = 1 - (frame_count & 1);
    parts[old_pfree].flag[PartFlags::IS_ENERGY]    = is_energy;

    parts[old_pfree].id = old_pfree;
    parts[old_pfree].type = type;
    parts[old_pfree].x = x;
    parts[old_pfree].y = y;
    parts[old_pfree].z = z;
    parts[old_pfree].rx = x;
    parts[old_pfree].ry = y;
    parts[old_pfree].rz = z;
    parts[old_pfree].vx = 0.0f;
    parts[old_pfree].vy = 0.0f;
    parts[old_pfree].vz = 0.0f;
    parts[old_pfree].assign_with_defaults(el.DefaultProperties);

    if (el.HeatConduct && HEAT_CONDUCT_CHANCE(frame_count, x, y, z, el.HeatConduct))
        heat.heat_map[z][y][x] = parts[old_pfree].temp;

    if (el.OnChangeType)
        el.OnChangeType(*this, old_pfree, x, y, z, PT_NONE, type);
    if (el.OnCreate)
        el.OnCreate(*this, old_pfree, x, y, z, type);

    if (paused) {
        if (_should_do_lighting(parts[old_pfree])) {
            graphics.ao_blocks[AO_FLAT_IDX(x, y, z)]++;
            _update_shadow_map(x, y, z);
        }
        parts_count++;
    }

    part_map[z][y][x] = PMAP(type, old_pfree);
    if (is_energy || !photons[z][y][x]) // Always try to show energy on top
        _set_color_data_at(x, y, z, &parts[old_pfree]);

    return old_pfree;
}

void Simulation::kill_part(const part_id i) {
    auto &part = parts[i];
    if (part.type <= 0) return;

    coord_t x = part.rx;
    coord_t y = part.ry;
    coord_t z = part.rz;

    if (pmap[z][y][x] && ID(pmap[z][y][x]) == i)
        pmap[z][y][x] = 0;
    else if (photons[z][y][x] && ID(photons[z][y][x]) == i)
        photons[z][y][x] = 0;

    graphics.color_force_update[FLAT_IDX(x, y, z)] = true; // Force whatever was below the particle to render now that it's gone
    if (GetElements()[part.type].OnChangeType)
        GetElements()[part.type].OnChangeType(*this, i, x, y, z, parts[i].type, PT_NONE);

    if (paused) {
        if (_should_do_lighting(part))
            graphics.ao_blocks[AO_FLAT_IDX(x, y, z)]--;
        graphics.shadows_force_update = true;
        parts_count--;
    }

    part.type = PT_NONE;
    part.flag[PartFlags::IS_ENERGY] = 0;
    heat.update_temperate(x, y, z, -1.0f);

    _set_color_data_at(x, y, z, nullptr);

    // Update PFREE pointer in linked list
    util::unique_spinlock _lock(parts_add_remove_lock);
    if (i == maxId && i > 0)
        maxId--;
    part.id = -pfree;
    pfree = i;
}

bool Simulation::part_change_type(const part_id i, const part_type new_type) {
    const auto &el = GetElements()[new_type];
    if (!el.Enabled || new_type == PT_NONE) {
		kill_part(i);
		return true;
	}

    const part_type prev_part_type = parts[i].type;
    const coord_t x = parts[i].rx;
    const coord_t y = parts[i].ry;
    const coord_t z = parts[i].rz;
    const bool is_energy = el.State == ElementState::TYPE_ENERGY;
    
    if (el.CreateAllowed && !el.CreateAllowed(*this, i, x, y, z, new_type))
        return false;
    if (GetElements()[parts[i].type].OnChangeType)
        GetElements()[parts[i].type].OnChangeType(*this, i, x, y, z, prev_part_type, new_type);
    if (el.OnChangeType)
        el.OnChangeType(*this, i, x, y, z, prev_part_type, new_type);

    if (paused && _should_do_lighting(parts[i]))
        graphics.shadows_force_update = true;

    parts[i].type = new_type;

	if (is_energy) {
		photons[z][y][x] = PMAP(new_type, i);
		if (pmap[z][y][x] && ID(pmap[z][y][x]) == i)
			pmap[z][y][x] = 0;
	}
	else {
		pmap[z][y][x] = PMAP(new_type, i);
		if (photons[z][y][x] && ID(photons[z][y][x]) == i)
			photons[z][y][x] = 0;
	}

    parts[pfree].flag[PartFlags::UPDATE_FRAME] = 1 - (frame_count & 1);
    parts[pfree].flag[PartFlags::MOVE_FRAME]   = 1 - (frame_count & 1);
    parts[pfree].flag[PartFlags::IS_ENERGY]    = is_energy;

    if (paused && _should_do_lighting(parts[i])) // Do this check again since type changed
        graphics.shadows_force_update = true;

    _set_color_data_at(x, y, z, &parts[i]);

	return true;
}

void Simulation::update_zslice(const coord_t pz) {
    if (pz < 1 || pz >= ZRES - 1)
        return;

    // Dirty rect does not have any impact on performance
    // for these sizes of YRES / XRES (could slow/speed up by a factor of a few ns)
    coord_t y1, y2;
    coord_t golz2 = (pz - 1) == 0 ? ZRES - 2 : pz - 1; // Check neighbors for gol as well, needs to wrap
    coord_t golz3 = (pz + 1) == ZRES - 1 ? 1 : pz + 1;

    if (!gol.zsliceHasGol[pz] && !gol.zsliceHasGol[golz2] && !gol.zsliceHasGol[golz3]) { // No GOL, can use smaller dirty rect
        y1 = min_y_per_zslice[pz - 1];
        y2 = max_y_per_zslice[pz - 1];
    } else {
        // Dirty rect is ignored to allow GOL to propagate
        // As GOL can wrap around this is the easiest way to prevent
        // synchronization errors with gol_map (Believe me I tried)
        y1 = 1;
        y2 = YRES - 1;
    }

    for (coord_t py = y1; py < y2; py++)
    for (coord_t px = 1; px < XRES - 1; px++) {
        if (pmap[pz][py][px]) {
            if (TYP(pmap[pz][py][px]) == PT_GOL && !gol.gol_map[pz][py][px]) { // Kill GOL that should die
                auto id = ID(pmap[pz][py][px]);
                parts[id].tmp1--;

                if (parts[id].tmp1 == 0) kill_part(id);
                else gol.gol_map[pz][py][px] = 1; // TODO gol type id
            } else {
                gol.gol_map[pz][py][px] = 0;
                update_part(ID(pmap[pz][py][px]));
            }
        }
        else if (gol.gol_map[pz][py][px]) { // Place gol if empty and should have a gol
            part_id i = create_part(px, py, pz, PT_GOL); // TODO: assign type
            if (i >= 0) {
                parts[i].flag[PartFlags::UPDATE_FRAME] = frame_count & 1;
            }
        }

        if (photons[pz][py][px])
            update_part(ID(photons[pz][py][px]));
    }
}

void Simulation::update_part(const part_id i, const bool consider_causality) {
    auto &part = parts[i];

    #ifdef DEBUG
    if (!part.type) throw std::runtime_error("update_part() called on NONE type particle");
    #endif

    // Since a particle might move we might update it again
    // if it moves in the direction of scanning the pmap array
    // To avoid this, it stores the parity of the frame count
    // and only updates the particle if the last frame it was updated
    // has the same parity
    const auto frame_count_parity = frame_count & 1;
    const coord_t x = part.rx;
    const coord_t y = part.ry;
    const coord_t z = part.rz;

    // Update causality constraint: depends on move_behavior and update step
    // Velocity can be set but the particle cannot move beyond its causality radius
    if (part.flag[PartFlags::UPDATE_FRAME] != frame_count_parity) { // Need to update
        const auto &el = GetElements()[part.type];
        if (consider_causality && el.Causality > max_ok_causality_range)
            return;

        part.flag[PartFlags::UPDATE_FRAME] = frame_count_parity > 0;

        // Life decrement and kill
        if ((el.Properties & ElementProperties::LIFE_DEC) && part.life > 0)
            part.life--;
        if ((el.Properties & ElementProperties::LIFE_KILL) && part.life <= 0) {
            kill_part(part.id);
            return;
        }

        // Air acceleration
        part.vx *= el.Loss;
        part.vy *= el.Loss;
        part.vz *= el.Loss;

        if (el.Advection) {
            const auto &airCell = air.cells[z / AIR_CELL_SIZE][y / AIR_CELL_SIZE][x / AIR_CELL_SIZE];
            part.vx += el.Advection * airCell.data[VX_IDX];
            part.vy += el.Advection * airCell.data[VY_IDX];
            part.vz += el.Advection * airCell.data[VZ_IDX];
        }

        if (el.Update) {
            const auto result = el.Update(*this, i, x, y, z, parts, pmap);
            if (result == -1) return;
        }
        move_behavior(i); // Apply element specific movement, like powder / liquid spread
    }

    // Movement causality constraint: depends on velocity
    if (part.flag[PartFlags::MOVE_FRAME] != frame_count_parity) { // Need to move
        // Causality check only needs to consider Z direction
        // since threads operate on XY slices, so only z velocity will
        // move a part into another thread domain
        if (consider_causality && fabs(part.vz) > max_ok_causality_range)
            return;
        part.flag[PartFlags::MOVE_FRAME] = frame_count_parity > 0;

        if (part.vx || part.vy || part.vz)
            _raycast_movement(i, x, y, z); // Apply velocity to displacement
    }
}

void Simulation::update() {
    if (paused) {
        if (graphics.shadows_force_update)
            _force_update_all_shadows();
        return;
    }

    auto t = GetTime();
    if (gol.golCount) gol.wait_and_get();
    download_heat_from_gpu();
    
    auto end = (GetTime() - t);
    // std::cout << end << "\n";

    // air.update(); // TODO

    #pragma omp parallel num_threads(sim_thread_count)
    { 
        const int thread_count = omp_get_num_threads();
        const int z_chunk_size = (ZRES - 2) / (2 * thread_count) + 1;
        const int tid = omp_get_thread_num();
        coord_t z_start = z_chunk_size * (2 * tid);

        if (tid == 0)
            actual_thread_count = thread_count;

        for (coord_t z = z_start; z < z_chunk_size + z_start; z++)
            update_zslice(z);

        #pragma omp barrier // Synchronize threads before processing 2nd chunk

        z_start = z_chunk_size * (2 * tid + 1);
        for (coord_t z = z_start; z < z_chunk_size + z_start; z++)
            update_zslice(z);
    }

    recalc_free_particles();
    frame_count++;
}

void Simulation::download_heat_from_gpu() {
    // Only download if we uploaded heat values at least once otherwise
    // all we'll get is 0s
    if (heat.uploadedOnce) {
        heat.wait_and_get();

        #pragma omp parallel for
        for (int i = 0; i < maxId; i++) {
            coord_t x = parts[i].rx;
            coord_t y = parts[i].ry;
            coord_t z = parts[i].rz;

            if (parts[i].type && heat.heat_map[z][y][x] >= 0.0f && GetElements()[parts[i].type].HeatConduct) {
                // && HEAT_CONDUCT_CHANCE(frame_count, x, y, z, GetElements()[parts[i].type].HeatConduct)
                parts[i].temp = util::clampf(heat.heat_map[z][y][x], MIN_TEMP, MAX_TEMP);
                
                // Heat transition
                const auto &el = GetElements()[parts[i].type];
                const part_type prevType = parts[i].type;
                part_type toType = PT_NONE;
                bool transition = false;

                if (el.HighTemperatureTransition != Transition::NONE && parts[i].temp > el.HighTemperature) {
                    toType = el.HighTemperatureTransition == Transition::TO_CTYPE ? parts[i].ctype : el.HighTemperatureTransition;
                    transition = true;
                    part_change_type(i, toType);
                }
                else if (el.LowTemperatureTransition != Transition::NONE && parts[i].temp < el.LowTemperature) {
                    toType = el.LowTemperatureTransition == Transition::TO_CTYPE ? parts[i].ctype : el.LowTemperatureTransition;
                    transition = true;
                    part_change_type(i, toType);
                }

                if (transition) {
                    // If transitioning to these types, set ctype to original type
                    if (toType == PT_ICE || toType == PT_LAVA)
                        parts[i].ctype = prevType;
                    // And clear ctype if transitioning from these types
                    else if (prevType == PT_ICE || prevType == PT_LAVA)
                        parts[i].ctype = PT_NONE;
                }
            }
        }
        heat.reset_dirty_chunks();
    }
}

void Simulation::recalc_free_particles() {
    parts_count = 0;
    part_id newMaxId = 0;

    std::fill(&max_y_per_zslice[0], &max_y_per_zslice[ZRES - 2], 0);
    std::fill(&min_y_per_zslice[0], &min_y_per_zslice[ZRES - 2], YRES - 1);
    memset(&graphics.shadow_map[0][0], 0, sizeof(graphics.shadow_map));
    graphics.ao_blocks.fill(0);
    gol.zsliceHasGol.fill(false);

    // First do all movement updates, which can create new holes in the pmap
    // Then we fix the holes afterwards
    for (part_id i = 1; i <= maxId; i++) {
        auto &part = parts[i];
        if (!part.type) continue;
        update_part(i, false);
    }

    for (part_id i = 0; i <= maxId; i++) {
        auto &part = parts[i];
        if (!part.type) continue;

        const coord_t x = part.rx;
        const coord_t y = part.ry;
        const coord_t z = part.rz;

        parts_count++;
        newMaxId = i;

        // Heat map update
        auto heatConduct = GetElements()[part.type].HeatConduct;
        if (heatConduct && HEAT_CONDUCT_CHANCE(frame_count, x, y, z, heatConduct))
            heat.update_temperate(x, y, z, util::clampf(part.temp, MIN_TEMP, MAX_TEMP));

        // Ambient occlusion and shadow rules
        if (part.id == ID(pmap[z][y][x]) && _should_do_lighting(part)) {
            graphics.ao_blocks[AO_FLAT_IDX(x, y, z)]++;
            _update_shadow_map(x, y, z);
        }

        // GOL check
        if (TYP(pmap[z][y][x]) == PT_GOL)
            gol.zsliceHasGol[z] = true;

        // Pmap / other cache
        min_y_per_zslice[z - 1] = std::min(y, min_y_per_zslice[z - 1]);
        max_y_per_zslice[z - 1] = std::max(y, max_y_per_zslice[z - 1]);

        // Pmap and graphics
        auto &map = part.flag[PartFlags::IS_ENERGY] ? photons : pmap;
        if (GetElements()[part.type].Graphics) // TODO: always update if heat view for example
            _set_color_data_at(part.rx, part.ry, part.rz, &part);
        if (!map[z][y][x]) {
            map[z][y][x] = PMAP(part.type, i);
            _set_color_data_at(x, y, z, &part);
            graphics.color_force_update[FLAT_IDX(x, y, z)] = false;
        } else if (graphics.color_force_update[FLAT_IDX(x, y, z)]) {
            graphics.color_force_update[FLAT_IDX(x, y, z)] = false;
            _set_color_data_at(x, y, z, &part);
        }
    }

    maxId = newMaxId + 1;
}

void Simulation::dispatch_compute_shaders() {
    if (paused) return;
    if (gol.golCount) gol.dispatch();
    heat.dispatch();
}


// Octree & color data updates
// ------------------------
void Simulation::_set_color_data_at(const coord_t x, const coord_t y, const coord_t z, const Particle * part) {
    uint32_t new_color = 0;
    util::Bitset8 new_flags = 0;

    if (part != nullptr) {
        const auto &el = GetElements()[part->type];
        new_color = el.Color.as_ABGR();
        new_flags = util::Bitset8(el.GraphicsFlags);

        if (el.Graphics) {
            RGBA color_out;
            el.Graphics(*this, *part, part->rx, part->ry, part->rz, color_out, new_flags);
            new_color = color_out.as_ABGR();
        }

        // TODO heat
        // int t = std::min(255, (int)(255 * (part->temp / 400.0f)));
        // RGBA tmp(t, t, t, 255);
        // new_color = tmp.as_ABGR();
    }

    unsigned int idx = FLAT_IDX(x, y, z);
    if (graphics.color_data[idx] == new_color && graphics.color_flags[idx] == new_flags) return; // Color did not actually change

    auto &tree = graphics.octree_blocks[
        (x / OCTREE_BLOCK_DIM) + (y / OCTREE_BLOCK_DIM) * X_BLOCKS +
        (z / OCTREE_BLOCK_DIM) * X_BLOCKS * Y_BLOCKS];

    graphics.color_data_modified[idx / COLOR_DATA_CHUNK_SIZE] = 0xFF;
    graphics.color_data[idx] = new_color;
    graphics.color_flags[idx] = new_flags;
    tree.modified = 0xFF;

    if (new_color)
        tree.insert(x & (OCTREE_BLOCK_DIM - 1), y & (OCTREE_BLOCK_DIM - 1), z & (OCTREE_BLOCK_DIM - 1));
    else
        tree.remove(x & (OCTREE_BLOCK_DIM - 1), y & (OCTREE_BLOCK_DIM - 1), z & (OCTREE_BLOCK_DIM - 1));
}

void Simulation::_update_shadow_map(const coord_t x, const coord_t y, const coord_t z) {
    unsigned int proj_x = (static_cast<unsigned int>(x) + (ZRES - z)) / SHADOW_MAP_SCALE;
    unsigned int proj_y = (static_cast<unsigned int>(y) + (ZRES - z)) / SHADOW_MAP_SCALE;
    graphics.shadow_map[proj_y][proj_x] = std::max(graphics.shadow_map[proj_y][proj_x], static_cast<uint8_t>(z));
}

bool Simulation::_should_do_lighting(const Particle &part) {
    return !GetElements()[part.type].GraphicsFlags[GraphicsFlagsIdx::NO_LIGHTING];
}

void Simulation::_force_update_all_shadows() {
    memset(&graphics.shadow_map[0][0], 0, sizeof(graphics.shadow_map));
    graphics.shadows_force_update = false;

    #pragma omp parallel for
    for (part_id i = 0; i <= maxId; i++) {
        auto &part = parts[i];
        if (!part.type) continue;
        if (part.id == ID(pmap[part.rz][part.ry][part.rx]) && _should_do_lighting(part))
            _update_shadow_map(part.rx, part.ry, part.rz);
    }
}
