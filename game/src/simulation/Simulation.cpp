#include "Simulation.h"
#include "ElementClasses.h"
#include "ElementDefs.h"

#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>

Simulation::Simulation():
    paused(false)
{
    std::fill(&pmap[0][0][0], &pmap[ZRES][YRES][XRES], 0);
    // std::fill(&parts[0], &parts[NPARTS], 0);

    pfree = 1;
    maxId = 0;
}


void Simulation::create_part(uint x, uint y, uint z, ElementType type) {
    // Create new part
    // Note: should it allow creation off screen? 
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
}

void Simulation::update() {
    for (int i = 0; i <= maxId; i++)
    {
        auto &part = parts[i];
        if (!part.type) continue; // TODO: can probably be more efficient

        move_behavior(i);

    }
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
    // if (part.vx == 0.0f && part.vy == 0.0 && part.vz == 0.0) return; // No velocity

    int x = (int)(part.x + 0.5f);
    int y = (int)(part.y + 0.5f);
    int z = (int)(part.z + 0.5f);

    if (el.State == ElementState::TYPE_LIQUID || el.State == ElementState::TYPE_POWDER) {
        // TODO: gravity
        // If nothing below go straight down
        if (pmap[z][y - 1][x] == 0) {
            try_move(idx, x, y - 1, z);
            return;
        }

        // Check surroundings or below surroundings
        std::vector<std::tuple<int, int, int>> next;
        const int ylvl = el.State == ElementState::TYPE_LIQUID ? y : y - 1;

        if (y > 1 && pmap[z][y - 1][x] > 0) {
            for (int dx = -1; dx <= 1; dx++)
                for (int dz = -1; dz <= 1; dz++) {
                    if (!dx && !dz) continue;
                    if ((int)z + dz < 1 || (int)z + dz >= ZRES - 1 || (int)x + dx < 1 || (int)x + dx >= XRES - 1) continue;

                    if (pmap[z + dz][y][x + dx] == 0 && pmap[z + dz][ylvl][x + dx] == 0) {
                        auto n = std::make_tuple(x + dx, ylvl, z + dz);
                        next.push_back(n);
                    }
                }
            
            if (next.size()) {
                int j = rand() % next.size();
                x = std::get<0>(next[j]);
                y = std::get<1>(next[j]);
                z = std::get<2>(next[j]);
                try_move(idx, x, y, z);
            }
        }
    }
    else if (el.State == ElementState::TYPE_GAS) {
        std::vector<std::tuple<int, int, int>> next;
        for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
            for (int dz = -1; dz <= 1; dz++) {
                if (!dx && !dz) continue;
                if ((int)y + dy < 1 || (int)y + dy >= YRES - 1 || (int)z + dz < 1 || z + dz >= ZRES - 1 || (int)x + dx < 1 || x + dx >= XRES - 1) continue;

                if (pmap[z + dz][y + dy][x + dx] == 0) {
                    auto n = std::make_tuple(x + dx, y + dy, z + dz);
                    next.push_back(n);
                }
            }
        
        if (next.size()) {
            int j = rand() % next.size();
            x = std::get<0>(next[j]);
            y = std::get<1>(next[j]);
            z = std::get<2>(next[j]);
            try_move(idx, x, y, z);
        }
    }
}

/**
 * @brief Try to move a particle to target location
 *        Updates the pmap and particle properties
 * 
 * @param idx Index in parts
 * @param x Target x
 * @param y Target y
 * @param z Target z
 */
void Simulation::try_move(int idx, uint x, uint y, uint z) {
    // TODO: consider edge mode
    if (x < 1 || y < 1 || z < 1 || x >= XRES - 1 || y >= YRES - 1 || z >= ZRES - 1)
        return;
    
    // TODO: can swap
    if (pmap[z][y][x]) return;
    
    int oldx = (int)(parts[idx].x + 0.5f);
    int oldy = (int)(parts[idx].y + 0.5f);
    int oldz = (int)(parts[idx].z + 0.5f);
    pmap[oldz][oldy][oldx] = 0;

    // TODO: suffers rounding issues
    parts[idx].x = x;
    parts[idx].y = y;
    parts[idx].z = z;
    pmap[z][y][x] = parts[idx].id;
}


