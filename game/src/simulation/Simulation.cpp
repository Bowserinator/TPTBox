#include "Simulation.h"

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
    // Assume all parts are powders for now

    for (int i = 0; i <= maxId; i++)
    {
        auto &part = parts[i];
        if (!part.type) continue; // TODO: can probably be more efficient

        int z = (int)(part.z + 0.5f);
        int y = (int)(part.y + 0.5f);
        int x = (int)(part.x + 0.5f);
        pmap[z][y][x] = 0;

        std::vector<std::tuple<int, int, int>> next;

        if (y > 1 && pmap[z][y - 1][x] > 0) {
            // Scatter powder if empty space around
            for (int dx = -1; dx <= 1; dx++)
                for (int dz = -1; dz <= 1; dz++) {
                    if (!dx && !dz) continue;
                    if ((int)z + dz < 1 || (int)z + dz >= ZRES - 1 || (int)x + dx < 1 || (int)x + dx >= XRES - 1) continue;

                    if (pmap[z + dz][y - 1][x + dx] == 0) {
                        auto n = std::make_tuple(x + dx, y - 1, z + dz);
                        next.push_back(n);
                    }
                }
            
            if (next.size()) {
                int j = rand() % next.size();
                x = std::get<0>(next[j]);
                y = std::get<1>(next[j]);
                z = std::get<2>(next[j]);
            }
            // Gas
            /*
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
            }*/
        } else if (y > 1) {
            // Move element down
            y -= 1;
        }

        // Move element to new pos
        part.x = x;
        part.y = y;
        part.z = z;
        pmap[z][y][x] = part.id;
    }
}