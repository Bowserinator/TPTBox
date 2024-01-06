#include "Air.h"

#include <algorithm>
#include <memory>
#include <iostream>
#include <cstring>

// How much to reduce on edges
constexpr float PRESSURE_MULTI = 0.9f;
constexpr float VELOCITY_MULTI = 0.8f;

constexpr float AIR_TSTEPP = 0.3f;
constexpr float AIR_TSTEPV = 0.4f;
constexpr float AIR_VADV   = 0.3f;
constexpr float AIR_VLOSS  = 0.999f;  // Vel reduction / frame
constexpr float AIR_PLOSS  = 0.9999f;
constexpr float ADV_DISTANCE_MULT = 0.7f;

/**
 * 3x3x3 gaussian blur kernel
 * Since the kernel is symmetric, there are only three values. If we imagine
 * the kernel in layers, where (distances = manhattan distance to center)
 * 
 * A = dis = 1 = KERNEL_ADJ
 * B = dis = 3 = KERNEL_CORNER2
 * C = dis = 2 = KERNEL_CORNER1
 * X = dis = 0 = KERNEL_CENTER
 * 
 * BCB (top and bottom)
 * CAC
 * BCB
 * 
 * CAC (middle slice)
 * AXA
 * CAC
 */
constexpr float SCALE = 1.7250451807374247; // Normalize so all terms of kernel sum to 1.0f
constexpr float KERNEL_CORNER1 = 0.058549832f / SCALE;
constexpr float KERNEL_CORNER2 = 0.035512268f / SCALE;
constexpr float KERNEL_ADJ  = 0.096532352f / SCALE;
constexpr float KERNEL_MID = 0.15915494f / SCALE;


Air::Air(Simulation &sim): sim(sim) {
    clear();
}

void Air::clear() {
    memset(cells, 0.0f, sizeof(cells));
    memset(out_cells, 0.0f, sizeof(cells));
}

void Air::update() {
    setEdgesAndWalls();
    setPressureFromVelocity();
    setVelocityFromPressure();
    diffusion();
    advection();

    std::swap(cells, out_cells);
}


void Air::setEdgesAndWalls() {
    // Reduce pressure and velocity on edges
    auto reduce_cell = [this](coord_t x, coord_t y, coord_t z) {
        cells[z][y][x].data[PRESSURE_IDX] *= PRESSURE_MULTI;
        cells[z][y][x].data[VX_IDX] *= VELOCITY_MULTI;
        cells[z][y][x].data[VY_IDX] *= VELOCITY_MULTI;
        cells[z][y][x].data[VZ_IDX] *= VELOCITY_MULTI;
    };

    // Top and bottom faces (Y axis)
    for (auto z = 0; z < AIR_ZRES; z++) {
        for (auto x = 0; x < AIR_XRES; x++)
            reduce_cell(x, 0, z);
        for (auto x = 0; x < AIR_XRES; x++)
            reduce_cell(x, 1, z);
        for (auto x = 0; x < AIR_XRES; x++)
            reduce_cell(x, AIR_YRES - 2, z);
        for (auto x = 0; x < AIR_XRES; x++)
            reduce_cell(x, AIR_YRES - 1, z);
    }

    // Z axis
    for (auto y = 0; y < AIR_YRES; y++) {
        for (auto x = 0; x < AIR_XRES; x++)
            reduce_cell(x, y, 0);
        for (auto x = 0; x < AIR_XRES; x++)
            reduce_cell(x, y, 1);
        for (auto x = 0; x < AIR_XRES; x++)
            reduce_cell(x, y, AIR_ZRES - 2);
        for (auto x = 0; x < AIR_XRES; x++)
            reduce_cell(x, y, AIR_ZRES - 1);
    }

    // X axis
    for (auto z = 0; z < AIR_ZRES; z++) {
        for (auto y = 0; y < AIR_YRES; y++) {
            reduce_cell(0, y, z);
            reduce_cell(1, y, z);
            reduce_cell(AIR_XRES - 2, y, z);
            reduce_cell(AIR_XRES - 1, y, z);
        }
    }

    // TODO: apply walls
}

void Air::setPressureFromVelocity() {
    for (auto z = 1; z < AIR_ZRES - 1; z++)
    for (auto y = 1; y < AIR_YRES - 1; y++)
    for (auto x = 1; x < AIR_XRES - 1; x++) {
        float dp = 
              cells[z][y][x - 1].data[VX_IDX] - cells[z][y][x + 1].data[VX_IDX]
            + cells[z][y - 1][x].data[VY_IDX] - cells[z][y + 1][x].data[VY_IDX]
            + cells[z - 1][y][x].data[VZ_IDX] - cells[z + 1][y][x].data[VZ_IDX];

        cells[z][y][x].data[PRESSURE_IDX] *= AIR_PLOSS;
        cells[z][y][x].data[PRESSURE_IDX] += dp * AIR_TSTEPP * 0.5f;
    }   
}

void Air::setVelocityFromPressure() {
    for (auto z = 1; z < AIR_ZRES - 1; z++)
    for (auto y = 1; y < AIR_YRES - 1; y++)
    for (auto x = 1; x < AIR_XRES - 1; x++) {
        float dx = cells[z][y][x - 1].data[PRESSURE_IDX] - cells[z][y][x + 1].data[PRESSURE_IDX];
        float dy = cells[z][y - 1][x].data[PRESSURE_IDX] - cells[z][y + 1][x].data[PRESSURE_IDX];
        float dz = cells[z - 1][y][x].data[PRESSURE_IDX] - cells[z + 1][y][x].data[PRESSURE_IDX];

        cells[z][y][x].data[VX_IDX] = cells[z][y][x].data[VX_IDX] * AIR_VLOSS + dx * AIR_TSTEPV * 0.5f;
        cells[z][y][x].data[VY_IDX] = cells[z][y][x].data[VY_IDX] * AIR_VLOSS + dy * AIR_TSTEPV * 0.5f;
        cells[z][y][x].data[VZ_IDX] = cells[z][y][x].data[VZ_IDX] * AIR_VLOSS + dz * AIR_TSTEPV * 0.5f;

        // TODO: set vel to 0 if wall adjacent or on cell
    }
}

void Air::diffusion() {
    // TODO: also blur on the edges, but edges assume 0 outside?

    for (auto z = 1; z < AIR_ZRES - 1; z++)
    for (auto y = 1; y < AIR_YRES - 1; y++)
    for (auto x = 1; x < AIR_XRES - 1; x++) {
        for (auto property = 0; property < 4; property++) {
            float corners1 = 
                  cells[z - 1][y][x - 1].data[property] + cells[z - 1][y][x + 1].data[property]
                + cells[z + 1][y][x - 1].data[property] + cells[z + 1][y][x + 1].data[property] // Middle slice
                + cells[z][y - 1][x - 1].data[property] + cells[z][y - 1][x + 1].data[property]
                + cells[z - 1][y - 1][x].data[property] + cells[z + 1][y - 1][x].data[property] // Bot slice
                + cells[z][y + 1][x - 1].data[property] + cells[z][y + 1][x + 1].data[property]
                + cells[z - 1][y + 1][x].data[property] + cells[z + 1][y + 1][x].data[property]; // Top slice
            float corners2 = cells[z - 1][y - 1][x - 1].data[property] + cells[z - 1][y - 1][x + 1].data[property]
                + cells[z + 1][y - 1][x - 1].data[property] + cells[z + 1][y - 1][x + 1].data[property]
                + cells[z - 1][y + 1][x - 1].data[property] + cells[z - 1][y + 1][x + 1].data[property]
                + cells[z + 1][y + 1][x - 1].data[property] + cells[z + 1][y + 1][x + 1].data[property];
            float adjacent = cells[z - 1][y][x].data[property] + cells[z + 1][y][x].data[property]
                + cells[z][y - 1][x].data[property] + cells[z][y + 1][x].data[property]
                + cells[z][y][x - 1].data[property] + cells[z][y][x + 1].data[property];

            out_cells[z][y][x].data[property] = corners1 * KERNEL_CORNER1 + corners2 * KERNEL_CORNER2
                + adjacent * KERNEL_ADJ + cells[z][y][x].data[property] * KERNEL_MID;
        }
    }
}

void Air::advection() {
    for (auto z = 1; z < AIR_ZRES - 1; z++)
    for (auto y = 1; y < AIR_YRES - 1; y++)
    for (auto x = 1; x < AIR_XRES - 1; x++) {
        auto dx = out_cells[z][y][x].data[VX_IDX];
        auto dy = out_cells[z][y][x].data[VY_IDX];
        auto dz = out_cells[z][y][x].data[VZ_IDX];

        auto tx = x - dx * ADV_DISTANCE_MULT;
        auto ty = y - dy * ADV_DISTANCE_MULT;
        auto tz = z - dz * ADV_DISTANCE_MULT;

        auto txi = (int)tx;
        auto tyi = (int)ty;
        auto tzi = (int)tz;

        tx -= txi;
        ty -= tyi;
        tz -= tzi;

        // TODO: wall check here
        if (txi >= 2 && txi <= AIR_XRES - 3 && tyi >= 2 && tyi <= AIR_YRES - 3 && tzi >= 2 && tzi <= AIR_ZRES - 3) {
            dx *= 1.0f - AIR_VADV;
            dy *= 1.0f - AIR_VADV;
            dz *= 1.0f - AIR_VADV;

            float y_mult = 1.0f - ty;

            // Linearly interpolate between 8 neighbors :sweat:
            for (int i = 0; i < 2; i++) {
                if (i == 1) y_mult = ty;

                dx += AIR_VADV * (1.0f - tx) * y_mult * (1.0f - tz) * cells[tzi][tyi + i][txi].data[VX_IDX];
                dy += AIR_VADV * (1.0f - tx) * y_mult * (1.0f - tz) * cells[tzi][tyi + i][txi].data[VY_IDX];
                dz += AIR_VADV * (1.0f - tx) * y_mult * (1.0f - tz) * cells[tzi][tyi + i][txi].data[VZ_IDX];

                dx += AIR_VADV * tx * y_mult * (1.0f - tz) * cells[tzi][tyi + i][txi + 1].data[VX_IDX];
                dy += AIR_VADV * tx * y_mult * (1.0f - tz) * cells[tzi][tyi + i][txi + 1].data[VY_IDX];
                dz += AIR_VADV * tx * y_mult * (1.0f - tz) * cells[tzi][tyi + i][txi + 1].data[VZ_IDX];

                dx += AIR_VADV * (1.0f - tx) * y_mult * tz * cells[tzi + 1][tyi + i][txi].data[VX_IDX];
                dy += AIR_VADV * (1.0f - tx) * y_mult * tz * cells[tzi + 1][tyi + i][txi].data[VY_IDX];
                dz += AIR_VADV * (1.0f - tx) * y_mult * tz * cells[tzi + 1][tyi + i][txi].data[VZ_IDX];

                dx += AIR_VADV * tx * y_mult * tz * cells[tzi + 1][tyi + i][txi + 1].data[VX_IDX];
                dy += AIR_VADV * tx * y_mult * tz * cells[tzi + 1][tyi + i][txi + 1].data[VY_IDX];
                dz += AIR_VADV * tx * y_mult * tz * cells[tzi + 1][tyi + i][txi + 1].data[VZ_IDX];
            }
        }

        out_cells[z][y][x].data[VX_IDX] = dx;
        out_cells[z][y][x].data[VY_IDX] = dy;
        out_cells[z][y][x].data[VZ_IDX] = dz;
    }
}