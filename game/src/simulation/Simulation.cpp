#include "Simulation.h"
#include "ElementClasses.h"
#include "ElementDefs.h"
#include "../util/vector_op.h"
#include "../util/util.h"

#include <omp.h>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>
#include <cmath>
#include <limits>

Simulation::Simulation():
    paused(false),
    air(*this)
{
    std::fill(&pmap[0][0][0], &pmap[ZRES][YRES][XRES], 0);
    std::fill(&photons[0][0][0], &photons[ZRES][YRES][XRES], 0);
    // std::fill(&parts[0], &parts[NPARTS], 0);

    pfree = 1;
    maxId = 0;
    frame_count = 0;
    parts_count = 0;
    

    // ---- Threads ------
    constexpr int MIN_CASUALITY_RADIUS = 4; // Width of each slice = 2 * this
    constexpr int MAX_THREADS = ZRES / (4 * MIN_CASUALITY_RADIUS); // Threads = number of slices / 2

    sim_thread_count = std::min(omp_get_max_threads(), MAX_THREADS);
    zslice_dirty_rects = std::vector<DirtyRect<coord_t>>(ZRES);
    
    // TODO: singleton?
    _init_can_move();
}

void Simulation::_init_can_move() {
    ElementType movingType, destinationType;
    const auto &elements = GetElements();

    for (movingType = 1; movingType <= ELEMENT_COUNT; movingType++) {
		for (destinationType = 1; destinationType <= ELEMENT_COUNT; destinationType++) {
            // Heavier elements can swap with lighter ones
			if (elements[movingType].Weight > elements[destinationType].Weight)
				can_move[movingType][destinationType] = PartSwapBehavior::SWAP;

            if (elements[movingType].State == ElementState::TYPE_ENERGY && elements[destinationType].State == ElementState::TYPE_ENERGY)
                can_move[movingType][destinationType] = PartSwapBehavior::OCCUPY_SAME;
        }
    }
}

part_id Simulation::create_part(const coord_t x, const coord_t y, const coord_t z, const ElementType type) {
    #ifdef DEBUG
    if (REVERSE_BOUNDS_CHECK(x, y, z))
        throw std::invalid_argument("Input to sim.create_part must be in bounds, got " +
            std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z));
    #endif

    const auto part_map = GetElements()[type].State == ElementState::TYPE_ENERGY ?
        photons : pmap;

    if (part_map[z][y][x]) return PartErr::ALREADY_OCCUPIED;
    if (pfree >= NPARTS) return PartErr::PARTS_FULL;

    // Create new part
    // Note: should it allow creation off screen? TODO
    const part_id next_pfree = parts[pfree].id < 0 ? -parts[pfree].id : pfree + 1;
    const part_id old_pfree = pfree;

    parts[pfree].id = pfree;
    parts[pfree].type = type;
    parts[pfree].x = x;
    parts[pfree].y = y;
    parts[pfree].z = z;
    parts[pfree].vx = 0.0f;
    parts[pfree].vy = 0.0f;
    parts[pfree].vz = 0.0f;

    part_map[z][y][x] = PMAP(type, pfree);
    maxId = std::max(maxId, pfree + 1);
    pfree = next_pfree;
    return old_pfree;
}

void Simulation::kill_part(const part_id i) {
    auto &part = parts[i];
    if (part.type <= 0) return;

    coord_t x = util::roundf(part.x);
    coord_t y = util::roundf(part.y);
    coord_t z = util::roundf(part.z);

    if (pmap[z][y][x] && ID(pmap[z][y][x]) == i)
        pmap[z][y][x] = 0;
    else if (photons[z][y][x] && ID(photons[z][y][x]) == i)
        photons[z][y][x] = 0;

    part.type = PT_NONE;

    if (i == maxId && i > 0)
        maxId--;

    part.id = -pfree;
    pfree = i;
}

void Simulation::update_zslice(const coord_t pz) {
    const auto &rect = zslice_dirty_rects[pz];
    for (coord_t py = rect.minY; py <= rect.maxY; py++)
    for (coord_t px = rect.minX; px <= rect.maxX; px++) {
        for (int j = 0; j < 2; j++) {
            auto map = j == 0 ? pmap : photons;
            if (!map[pz][py][px]) continue;

            part_id i = ID(map[pz][py][px]);
            update_part(i);
        }
    }
}

void Simulation::update_part(const part_id i) {
    auto &part = parts[i];

    // Since a particle might move we might update it again
    // if it moves in the direction of scanning the pmap array
    // To avoid this, it stores the parity of the frame count
    // and only updates the particle if the last frame it was updated
    // has the same parity
    const auto frame_count_parity = frame_count & 1;
    if (part.flag[PartFlags::UPDATE_FRAME] == frame_count_parity)
        return;
    part.flag[PartFlags::UPDATE_FRAME] = frame_count_parity;

    const coord_t x = util::roundf(part.x);
    const coord_t y = util::roundf(part.y);
    const coord_t z = util::roundf(part.z);

    // Air acceleration
    const auto &el = GetElements()[part.type];
    if (el.Advection) {
        const auto &airCell = air.cells[z / AIR_CELL_SIZE][y / AIR_CELL_SIZE][x / AIR_CELL_SIZE];
        part.vx += el.Advection * airCell.data[VX_IDX];
        part.vy += el.Advection * airCell.data[VY_IDX];
        part.vz += el.Advection * airCell.data[VZ_IDX];
    }

    if (el.Update) {
        const auto result = el.Update(this, i, x, y, z, parts, pmap);
        if (result == -1) return; // TODO: flags or something
    }

    if (part.vx || part.vy || part.vz)
        _raycast_movement(i, x, y, z); // Apply velocity to displacement
    move_behavior(i); // Apply element specific movement, like powder / liquid spread
}

void Simulation::update() {
    // air.update(); // TODO

    // omp_set_dynamic(0);
    // omp_set_num_threads(omp_get_max_threads());

    #pragma omp parallel num_threads(sim_thread_count)
    { 
        const int z_chunk_size = ZRES / (2 * sim_thread_count) + 1;
        const int tid = omp_get_thread_num();
        coord_t z_start = z_chunk_size * (2 * tid);

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

void Simulation::recalc_free_particles() {
    parts_count = 0;
    part_id newMaxId = 0;

    for (auto &rect : zslice_dirty_rects)
        rect.reset();

    for (part_id i = 0; i <= maxId; i++) {
        auto &part = parts[i];
        if (!part.type) continue;

        parts_count++;
        newMaxId = i;

        const coord_t x = util::roundf(part.x);
        const coord_t y = util::roundf(part.y);
        const coord_t z = util::roundf(part.z);

        auto &map = GetElements()[part.type].State == ElementState::TYPE_ENERGY ? photons : pmap;
        if (!map[z][y][x])
            map[z][y][x] = PMAP(part.type, i);

        update_part(i);

        // After particle has moved, updated dirty rect
        // if the particle was not deleted
        if (part.type)
            zslice_dirty_rects[util::roundf(part.z)].update(util::roundf(part.x), util::roundf(part.y));
    }
    maxId = newMaxId + 1;
}
