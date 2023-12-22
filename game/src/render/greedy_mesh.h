#ifndef GREEDY_MESH_H
#define GREEDY_MESH_H

#include "raylib.h"
#include "rlgl.h"

#include "./camera/camera.h"
#include "../simulation/Simulation.h"
#include "constants.h"
#include "./types/bit_col.h"

#include <cstdint>
#include <vector>



void fast_greedy_mesh(Mesh &mesh, Simulation &sim, RenderCamera & camera,
        unsigned int &vertex_count,
        unsigned int &color_count) {
    auto pushVertices = [&mesh, &color_count, &vertex_count](unsigned char red, float x, float y, float z) {
        mesh.vertices[vertex_count] = x;
        mesh.vertices[vertex_count + 1] = y;
        mesh.vertices[vertex_count + 2] = z;

        vertex_count += 3;
        mesh.colors[color_count] = red;
        mesh.colors[color_count + 1] = 0;
        mesh.colors[color_count + 2] = 0xAA;
        mesh.colors[color_count + 3] = 0xFF;
        color_count += 4;
    };

    BitColumn frontFaces[ZRES][XRES]; // zx
    BitColumn backFaces[ZRES][XRES];  // zx

    BitColumn topFaces[YRES][XRES];  // yx
    BitColumn botFaces[YRES][XRES];  // yx

    BitColumn rightFaces[XRES][ZRES]; // xz
    BitColumn leftFaces[XRES][ZRES];  // xz

    for (unsigned int x = 0; x < XRES; x++)
        for (unsigned int z = 0; z < ZRES; z++) {
            frontFaces[z][x].bytes = std::vector<std::uint8_t>(YRES / 8 + 1, 0);
            backFaces[z][x].bytes = std::vector<std::uint8_t>(YRES / 8 + 1, 0);
        }
    for (unsigned int y = 0; y < YRES; y++)
        for (unsigned int x = 0; x < XRES; x++) {
            topFaces[y][x].bytes = std::vector<std::uint8_t>(ZRES / 8 + 1, 0);
            botFaces[y][x].bytes = std::vector<std::uint8_t>(ZRES / 8 + 1, 0);
        }
    for (unsigned int z = 0; z < ZRES; z++)
        for (unsigned int x = 0; x < XRES; x++) {
            rightFaces[x][z].bytes = std::vector<std::uint8_t>(YRES / 8 + 1, 0);
            leftFaces[x][z].bytes = std::vector<std::uint8_t>(YRES / 8 + 1, 0);
        }

    for (int i = 0; i < sim.maxId; i++) { // TODO: size_t doesn't exist??
        const auto &part = sim.parts[i];
        if (!part.id) continue;

        int px = (int)(sim.parts[i].x + 0.5f);
        int py = (int)(sim.parts[i].y + 0.5f);
        int pz = (int)(sim.parts[i].z + 0.5f);

        if (camera.sphereOutsideFrustum(px, py, pz, DIS_UNIT_CUBE_CENTER_TO_CORNER))
            continue;

        bool top = sim.pmap[pz][py + 1][px] == 0;
        bool bot = sim.pmap[pz][py - 1][px] == 0;
        bool back = sim.pmap[pz - 1][py][px] == 0;
        bool front = sim.pmap[pz + 1][py][px] == 0;
        bool left = sim.pmap[pz][py][px - 1] == 0;
        bool right = sim.pmap[pz][py][px + 1] == 0;

        if (front) frontFaces[pz][px].setY(py);
        if (back)  backFaces[pz][px].setY(py);
        if (top) topFaces[py][px].setY(pz);
        if (bot) botFaces[py][px].setY(pz);
        if (left) leftFaces[px][pz].setY(py);
        if (right) rightFaces[px][pz].setY(py);
    }

    for (unsigned int y = 0; y < YRES; y++) {
        for (unsigned int x = 0; x < XRES; x++) {
            for (unsigned int z = 0; z < ZRES; z++) {
                // Consume front faces
                if (frontFaces[z][x].getY(y)) {
                    int colYEnd = frontFaces[z][x].getContigiousEnd(y);
                    int x1 = x;
                    int x2 = x;
                    while (frontFaces[z][x1].isRangeAllOne(y, colYEnd) && x1 > 1)
                        x1--;
                    while (frontFaces[z][x2].isRangeAllOne(y, colYEnd) && x2 < XRES - 1)
                        x2++;
                    if (x1 < x) x1++;
                    if (x2 > x) x2--;

                    for (auto _x = x1; _x <= x2; _x++)
                        frontFaces[z][_x].unsetYRange(y, colYEnd);

                    float cx = (x1 + x2) / 2.0f;
                    float cy = (y + colYEnd) / 2.0f;
                    float width = x2 - x1 + 1;
                    float height = colYEnd - y + 1;
                    float length = 1.0f;
                    unsigned char red = 0xFF;

                    pushVertices(red, cx - width/2, cy - height/2, z + length/2);  // Bottom Left
                    pushVertices(red, cx + width/2, cy - height/2, z + length/2);  // Bottom Right
                    pushVertices(red, cx - width/2, cy + height/2, z + length/2);  // Top Left

                    pushVertices(red, cx + width/2, cy + height/2, z + length/2);  // Top Right
                    pushVertices(red, cx - width/2, cy + height/2, z + length/2);  // Top Left
                    pushVertices(red, cx + width/2, cy - height/2, z + length/2);  // Bottom Right
                }

                // Consume back faces
                if (backFaces[z][x].getY(y)) {
                    int colYEnd = backFaces[z][x].getContigiousEnd(y);
                    int x1 = x;
                    int x2 = x;
                    while (backFaces[z][x1].isRangeAllOne(y, colYEnd) && x1 > 1)
                        x1--;
                    while (backFaces[z][x2].isRangeAllOne(y, colYEnd) && x2 < XRES - 1)
                        x2++;
                    if (x1 < x) x1++;
                    if (x2 > x) x2--;

                    for (auto _x = x1; _x <= x2; _x++)
                        backFaces[z][_x].unsetYRange(y, colYEnd);

                    float cx = (x1 + x2) / 2.0f;
                    float cy = (y + colYEnd) / 2.0f;
                    float width = x2 - x1 + 1;
                    float height = colYEnd - y + 1;
                    float length = 1.0f;
                    unsigned char red = 0xFF;

                    pushVertices(red, cx - width/2, cy - height/2, z - length/2);  // Bottom Left
                    pushVertices(red, cx - width/2, cy + height/2, z - length/2);  // Top Left
                    pushVertices(red, cx + width/2, cy - height/2, z - length/2);  // Bottom Right

                    pushVertices(red, cx + width/2, cy + height/2, z - length/2);  // Top Right
                    pushVertices(red, cx + width/2, cy - height/2, z - length/2);  // Bottom Right
                    pushVertices(red, cx - width/2, cy + height/2, z - length/2);  // Top Left
                }

                // Consume top faces
                if (topFaces[y][x].getY(z)) {
                    int colZEnd = topFaces[y][x].getContigiousEnd(z);
                    int x1 = x;
                    int x2 = x;
                    while (topFaces[y][x1].isRangeAllOne(z, colZEnd) && x1 > 1)
                        x1--;
                    while (topFaces[y][x2].isRangeAllOne(z, colZEnd) && x2 < XRES - 1)
                        x2++;
                    if (x1 < x) x1++;
                    if (x2 > x) x2--;

                    for (auto _x = x1; _x <= x2; _x++)
                        topFaces[y][_x].unsetYRange(z, colZEnd);

                    float cx = (x1 + x2) / 2.0f;
                    float cz = (z + colZEnd) / 2.0f;
                    float width = x2 - x1 + 1;
                    float height = 1.0f;
                    float length = colZEnd - z + 1;
                    unsigned char red = 0xFF;

                    pushVertices(red, cx - width/2, y + height/2, cz - length/2);  // Top Left
                    pushVertices(red, cx - width/2, y + height/2, cz + length/2);  // Bottom Left
                    pushVertices(red, cx + width/2, y + height/2, cz + length/2);  // Bottom Right

                    pushVertices(red, cx + width/2, y + height/2, cz - length/2);  // Top Right
                    pushVertices(red, cx - width/2, y + height/2, cz - length/2);  // Top Left
                    pushVertices(red, cx + width/2, y + height/2, cz + length/2);  // Bottom Right
                }

                // Consume bot faces
                if (botFaces[y][x].getY(z)) {
                    int colZEnd = botFaces[y][x].getContigiousEnd(z);
                    int x1 = x;
                    int x2 = x;
                    while (botFaces[y][x1].isRangeAllOne(z, colZEnd) && x1 > 1)
                        x1--;
                    while (botFaces[y][x2].isRangeAllOne(z, colZEnd) && x2 < XRES - 1)
                        x2++;
                    if (x1 < x) x1++;
                    if (x2 > x) x2--;

                    for (auto _x = x1; _x <= x2; _x++)
                        botFaces[y][_x].unsetYRange(z, colZEnd);

                    float cx = (x1 + x2) / 2.0f;
                    float cz = (z + colZEnd) / 2.0f;
                    float width = x2 - x1 + 1;
                    float height = 1.0f;
                    float length = colZEnd - z + 1;
                    unsigned char red = 0xFF;

                    pushVertices(red, cx - width/2, y - height/2, cz - length/2);  // Top Left
                    pushVertices(red, cx + width/2, y - height/2, cz + length/2);  // Bottom Right
                    pushVertices(red, cx - width/2, y - height/2, cz + length/2);  // Bottom Left

                    pushVertices(red, cx + width/2, y - height/2, cz - length/2);  // Top Right
                    pushVertices(red, cx + width/2, y - height/2, cz + length/2);  // Bottom Right
                    pushVertices(red, cx - width/2, y - height/2, cz - length/2);  // Top Left
                }


                // Consume left faces
                if (leftFaces[x][z].getY(y)) {
                    int colYEnd = leftFaces[x][z].getContigiousEnd(y);
                    int z1 = z;
                    int z2 = z;
                    while (leftFaces[x][z1].isRangeAllOne(y, colYEnd) && z1 > 1)
                        z1--;
                    while (leftFaces[x][z2].isRangeAllOne(y, colYEnd) && z2 < ZRES - 1)
                        z2++;
                    if (z1 < z) z1++;
                    if (z2 > z) z2--;

                    for (auto _z = z1; _z <= z2; _z++)
                        leftFaces[x][_z].unsetYRange(y, colYEnd);

                    float cz = (z1 + z2) / 2.0f;
                    float cy = (y + colYEnd) / 2.0f;
                    float width = 1.0f;
                    float height = colYEnd - y + 1;
                    float length = z2 - z1 + 1;
                    unsigned char red = 0xFF;

                    pushVertices(red, x - width/2, cy - height/2, cz - length/2);  // Bottom Right
                    pushVertices(red, x - width/2, cy + height/2, cz + length/2);  // Top Left
                    pushVertices(red, x - width/2, cy + height/2, cz - length/2);  // Top Right

                    pushVertices(red, x - width/2, cy - height/2, cz + length/2);  // Bottom Left
                    pushVertices(red, x - width/2, cy + height/2, cz + length/2);  // Top Left
                    pushVertices(red, x - width/2, cy - height/2, cz - length/2);  // Bottom Right
                }

                // Consume right faces
                if (rightFaces[x][z].getY(y)) {
                    int colYEnd = rightFaces[x][z].getContigiousEnd(y);
                    int z1 = z;
                    int z2 = z;
                    while (rightFaces[x][z1].isRangeAllOne(y, colYEnd) && z1 > 1)
                        z1--;
                    while (rightFaces[x][z2].isRangeAllOne(y, colYEnd) && z2 < ZRES - 1)
                        z2++;
                    if (z1 < z) z1++;
                    if (z2 > z) z2--;

                    for (auto _z = z1; _z <= z2; _z++)
                        rightFaces[x][_z].unsetYRange(y, colYEnd);

                    float cz = (z1 + z2) / 2.0f;
                    float cy = (y + colYEnd) / 2.0f;
                    float width = 1.0f;
                    float height = colYEnd - y + 1;
                    float length = z2 - z1 + 1;
                    unsigned char red = 0xFF;

                    pushVertices(red, x + width/2, cy - height/2, cz - length/2);  // Bottom Right
                    pushVertices(red, x + width/2, cy + height/2, cz - length/2);  // Top Right
                    pushVertices(red, x + width/2, cy + height/2, cz + length/2);  // Top Left

                    pushVertices(red, x + width/2, cy - height/2, cz + length/2);  // Bottom Left
                    pushVertices(red, x + width/2, cy - height/2, cz - length/2);  // Bottom Right
                    pushVertices(red, x + width/2, cy + height/2, cz + length/2);  // Top Left
                }
            }
        }
    }
}


#endif