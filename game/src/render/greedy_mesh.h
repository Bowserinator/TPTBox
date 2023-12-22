#ifndef GREEDY_MESH_H
#define GREEDY_MESH_H

#include "raylib.h"
#include "rlgl.h"

#include "./camera/camera.h"
#include "../simulation/Simulation.h"
#include "constants.h"

#include <vector>


void greedy_mesh_front_back(Mesh &mesh, Simulation &sim, RenderCamera & camera,
        unsigned int &vertex_count,
        unsigned int &color_count,
        int zmulti) {

    std::vector<bool> visited(sim.maxId, false);

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

    for (int i = 0; i < sim.maxId; i++) { // TODO: size_t doesn't exist??
        if (visited[i]) continue;
        const auto &part = sim.parts[i];
        if (!part.id) continue;

        int px = (int)(sim.parts[i].x + 0.5f);
        int py = (int)(sim.parts[i].y + 0.5f);
        int pz = (int)(sim.parts[i].z + 0.5f);

        int endx = px;
        int endy = py;
        int endz = pz;

        unsigned char red = 0xFF;

        //if (camera.sphereOutsideFrustum(x, y, z, DIS_UNIT_CUBE_CENTER_TO_CORNER))
        //    continue;

        // Consume col
        auto id = sim.pmap[endz][endy][endx];
        while (endy < YRES - 1 && id > 0 && !visited[id] &&
                sim.pmap[endz + zmulti][endy][endx] == 0) {
            visited[id] = true;
            endy++;
            id = sim.pmap[endz][endy][endx];
        }

        // Empty face
        if (endy == py && endz == pz) continue;

        // Go left
        while (endx > 1) {
            bool flag = false;
            for (int y = py; y < endy; y++) {
                auto id = sim.pmap[endz][y][endx - 1];
                if (id == 0 || visited[id]|| sim.pmap[endz + zmulti][y][endx - 1] != 0) {
                    flag = true;
                    break;
                }
            }
            if(flag) break;
            for (int y = py; y < endy; y++)
                visited[sim.pmap[endz][y][endx - 1]] = 1;

            endx--;
        }

        // Go right
        while (px > 1) {
            bool flag = false;
            for (int y = py; y < endy; y++) {
                auto id = sim.pmap[endz][y][px + 1];
                if (id == 0 || visited[id]|| sim.pmap[endz + zmulti][y][px + 1] != 0) {
                    flag = true;
                    break;
                }
            }
            if(flag) break;
            for (int y = py; y < endy; y++)
                visited[sim.pmap[endz][y][px + 1]] = 1;

            px++;
        }


        //red = 0xAA;
        float x = (px + endx) / 2.0f;
        float y = (py + endy) / 2.0f - 0.5f;
        float z = (pz + endz) / 2.0f;
        float width = abs(endx - px) + 1;
        float height = abs(endy - py);
        float length = endz - pz + 1;

        if (zmulti > 0) {
            pushVertices(red, x - width/2, y - height/2, z + zmulti*length/2);  // Bottom Left
            pushVertices(red, x + width/2, y - height/2, z + zmulti*length/2);  // Bottom Right
            pushVertices(red, x - width/2, y + height/2, z + zmulti*length/2);  // Top Left

            pushVertices(red, x + width/2, y + height/2, z + zmulti*length/2);  // Top Right
            pushVertices(red, x - width/2, y + height/2, z + zmulti*length/2);  // Top Left
            pushVertices(red, x + width/2, y - height/2, z + zmulti*length/2);  // Bottom Right
        } else {
            pushVertices(red, x - width/2, y - height/2, z - length/2);  // Bottom Left
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Left
            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Bottom Right

            pushVertices(red, x + width/2, y + height/2, z - length/2);  // Top Right
            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Bottom Right
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Left
        }
    }
}

#endif