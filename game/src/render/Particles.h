#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"
#include "rlgl.h"
#include <omp.h>

#include "./camera/camera.h"
#include "../simulation/Simulation.h"
#include "../simulation/ElementClasses.h"
#include "../util/color.h"
#include "../util/util.h"
#include "constants.h"
#include "greedy_mesh.h"

#include <iostream>
#include <cstring>
#include <glad.h>


constexpr float width  = 1.0f;
constexpr float height = 1.0f;
constexpr float length = 1.0f;

static Mesh mesh = { 0 };
static bool found = false;

constexpr int BUFF_SIZE = sizeof(float) * 500000; // 10^6

void getMeshFromSim(Simulation &sim, RenderCamera & camera) {
    if (!found) {
        // SetTraceLogLevel(LOG_ERROR);

        mesh.vertices = (float *)MemAlloc(sizeof(float) * BUFF_SIZE * 3);    // 3 vertices, 3 coordinates each (x, y, z)
        mesh.texcoords = NULL; // (float *)MemAlloc(sizeof(float) * BUFF_SIZE * 3);   // 3 vertices, 2 coordinates each (x, y)
        mesh.colors = (unsigned char*)MemAlloc(BUFF_SIZE * 4);
    }
    unsigned int vertex_count = 0;
    unsigned int color_count = 0;
    unsigned int triangle_count = 0;


    auto pushVertices = [&triangle_count, &color_count, &vertex_count](RGBA color, float x, float y, float z) {
        mesh.vertices[vertex_count] = x;
        mesh.vertices[vertex_count + 1] = y;
        mesh.vertices[vertex_count + 2] = z;

        vertex_count += 3;
        mesh.colors[color_count] = color.r;
        mesh.colors[color_count + 1] = color.g;
        mesh.colors[color_count + 2] = color.b;
        mesh.colors[color_count + 3] = color.a;
        color_count += 4;
    };



    // fast_greedy_mesh(mesh, sim, camera, vertex_count, color_count);

    #pragma omp parallel for
    for (int i = 0; i < sim.maxId ; i++) { // sim.maxId
        const auto &part = sim.parts[i];
        if (!part.type) continue;
        int px = util::roundf(sim.parts[i].x);
        int py = util::roundf(sim.parts[i].y);
        int pz = util::roundf(sim.parts[i].z);

        float x = px;
        float y = py;
        float z = pz;

        if (camera.sphereOutsideFrustum(x, y, z, DIS_UNIT_CUBE_CENTER_TO_CORNER))
            continue;

        bool top = sim.pmap[pz][py + 1][px] == 0;
        bool bot = sim.pmap[pz][py - 1][px] == 0;
        bool back = sim.pmap[pz - 1][py][px] == 0;
        bool front = sim.pmap[pz + 1][py][px] == 0;
        bool left = sim.pmap[pz][py][px - 1] == 0;
        bool right = sim.pmap[pz][py][px + 1] == 0;

        auto color = GetElements()[sim.parts[i].type].Color;
        auto red = color;

        // Front face
        if (front) {
            red = color;
            red.darken(0.8);
            pushVertices(red, x - width/2, y - height/2, z + length/2);  // Bottom Left
            pushVertices(red, x + width/2, y - height/2, z + length/2);  // Bottom Right
            pushVertices(red, x - width/2, y + height/2, z + length/2);  // Top Left

            pushVertices(red, x + width/2, y + height/2, z + length/2);  // Top Right
            pushVertices(red, x - width/2, y + height/2, z + length/2);  // Top Left
            pushVertices(red, x + width/2, y - height/2, z + length/2);  // Bottom Right
        }

        // Back face
        if (back) {
            red = color;
            red.darken(0.8);
            pushVertices(red, x - width/2, y - height/2, z - length/2);  // Bottom Left
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Left
            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Bottom Right

            pushVertices(red, x + width/2, y + height/2, z - length/2);  // Top Right
            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Bottom Right
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Left
        }

        // Top face
        if (top) {
            red = color;
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Left
            pushVertices(red, x - width/2, y + height/2, z + length/2);  // Bottom Left
            pushVertices(red, x + width/2, y + height/2, z + length/2);  // Bottom Right

            pushVertices(red, x + width/2, y + height/2, z - length/2);  // Top Right
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Left
            pushVertices(red, x + width/2, y + height/2, z + length/2);  // Bottom Right
        }

        // Bottom face
        if (bot) {
            red = color;
            red.darken(0.6);
            pushVertices(red, x - width/2, y - height/2, z - length/2);  // Top Left
            pushVertices(red, x + width/2, y - height/2, z + length/2);  // Bottom Right
            pushVertices(red, x - width/2, y - height/2, z + length/2);  // Bottom Left

            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Top Right
            pushVertices(red, x + width/2, y - height/2, z + length/2);  // Bottom Right
            pushVertices(red, x - width/2, y - height/2, z - length/2);  // Top Left
        }

        // Right face
        if (right) {
            red = color;
            red.darken(0.9);
            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Bottom Right
            pushVertices(red, x + width/2, y + height/2, z - length/2);  // Top Right
            pushVertices(red, x + width/2, y + height/2, z + length/2);  // Top Left

            pushVertices(red, x + width/2, y - height/2, z + length/2);  // Bottom Left
            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Bottom Right
            pushVertices(red, x + width/2, y + height/2, z + length/2);  // Top Left
        }

        // Left face
        if (left) {
            red = color;
            red.darken(0.9);
            pushVertices(red, x - width/2, y - height/2, z - length/2);  // Bottom Right
            pushVertices(red, x - width/2, y + height/2, z + length/2);  // Top Left
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Right

            pushVertices(red, x - width/2, y - height/2, z + length/2);  // Bottom Left
            pushVertices(red, x - width/2, y + height/2, z + length/2);  // Top Left
            pushVertices(red, x - width/2, y - height/2, z - length/2);  // Bottom Right
        }
    }

    mesh.triangleCount = vertex_count / 9;
    mesh.vertexCount = vertex_count / 3;

    if (!found) {
        found = true;
        UploadMesh(&mesh, true);

        glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*BUFF_SIZE * 3, 0, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[3]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned char)*BUFF_SIZE * 4, 0, GL_DYNAMIC_DRAW);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[0]);
        void *data = glMapBufferRange( GL_ARRAY_BUFFER, 0, sizeof(float) * vertex_count, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        std::memcpy( data, &mesh.vertices[0], sizeof(float) * vertex_count);
        glUnmapBuffer(GL_ARRAY_BUFFER);

        glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[3]);
        data = glMapBufferRange( GL_ARRAY_BUFFER, 0, sizeof(char) * color_count, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        std::memcpy( data, &mesh.colors[0], sizeof(char) * color_count);
        glUnmapBuffer(GL_ARRAY_BUFFER);

        // glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[3]);
        // //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(char) * color_count, mesh.colors);
        // data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        // std::memcpy( data, mesh.colors, sizeof(unsigned char) * color_count);
        // glUnmapBuffer(GL_ARRAY_BUFFER);

        //UpdateMeshBuffer(mesh, 0, &mesh.vertices[0], sizeof(float) * vertex_count, 0);
        //UpdateMeshBuffer(mesh, 3, mesh.colors, sizeof(unsigned char) * color_count, 0);
        // UpdateMeshBuffer(mesh, 1, &texcoords[0], sizeof(float) * texcoords.size(), 0);
    }
}

void DrawCubeParticle(Simulation &sim, RenderCamera &camera, const Color color, const Color lineColor) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    
    getMeshFromSim(sim, camera);
    DrawMesh(mesh, LoadMaterialDefault(), MatrixIdentity()); 

    return;


    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < sim.maxId; i++) { // TODO: size_t doesn't exist??
        const auto &part = sim.parts[i];
        if (!part.type) continue;

        int px = util::roundf(sim.parts[i].x);
        int py = util::roundf(sim.parts[i].y);
        int pz = util::roundf(sim.parts[i].z);

        // TODO: const is dis to cube center
        if (camera.sphereOutsideFrustum(px, py, pz, DIS_UNIT_CUBE_CENTER_TO_CORNER))
            continue;

        bool top = sim.pmap[pz][py + 1][px] == 0;
        bool bot = sim.pmap[pz][py - 1][px] == 0;
        bool back = sim.pmap[pz - 1][py][px] == 0;
        bool front = sim.pmap[pz + 1][py][px] == 0;
        bool left = sim.pmap[pz][py][px - 1] == 0;
        bool right = sim.pmap[pz][py][px + 1] == 0;

        rlPushMatrix();
            // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
            rlTranslatef(px, py, pz);
                // Front face
                if (front) {
                    rlColor4ub(color.r * 0.9, color.g * 0.9, color.b * 0.9, color.a);
                    rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left
                    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
                    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left

                    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Right
                    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
                    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
                }

                // Back face
                if (back) {
                    rlColor4ub(color.r * 0.9, color.g * 0.9, color.b * 0.9, color.a);
                    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Left
                    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
                    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right

                    rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
                    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
                    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
                }

                // Top face
                if (top) {
                    rlColor4ub(color.r, color.g, color.b, color.a);
                    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
                    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Bottom Left
                    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Bottom Right

                    rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
                    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
                    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Bottom Right
                }

                // Bottom face
                if (bot) {
                    rlColor4ub(color.r * 0.7, color.g * 0.7, color.b * 0.7, color.a);
                    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Left
                    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
                    rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left

                    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Top Right
                    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
                    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Left
                }

                // Right face
                if (right) {
                    rlColor4ub(color.r * 0.8, color.g * 0.8, color.b * 0.8, color.a);
                    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
                    rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
                    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Left

                    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Left
                    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
                    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Left
                }

                // Left face
                if (left) {
                    rlColor4ub(color.r * 0.8, color.g * 0.8, color.b * 0.8, color.a);
                    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Right
                    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
                    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Right

                    rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left
                    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
                    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Right
                }
            /*
            rlBegin(RL_LINES);
                rlColor4ub(lineColor.r, lineColor.g, lineColor.b, lineColor.a);

                // Front face
                //------------------------------------------------------------------
                if (front) {
                    // Bottom line
                    if (bot) {
                        rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom left
                        rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom right
                    }

                    // Left line
                    if (right) {
                        rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom right
                        rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top right
                    }

                    // Top line
                    if (top) {
                        rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top right
                        rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top left
                    }

                    // Right line
                    if (left) {
                        rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top left
                        rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom left
                    }
                }

                // Back face
                //------------------------------------------------------------------
                if (back) {
                    // Bottom line
                    if (bot) {
                        rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom left
                        rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom right
                    }

                    // Left line
                    if (right) {
                        rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom right
                        rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top right
                    }

                    // Top line
                    if (top) {
                        rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top right
                        rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top left
                    }

                    // Right line
                    if (left) {
                        rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top left
                        rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom left
                    }
                }

                // Top face
                //------------------------------------------------------------------
                if (top) {
                    // Left line
                    if (left) {
                        rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top left front
                        rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top left back
                    }

                    // Right line
                    if (right) {
                        rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top right front
                        rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top right back
                    }
                }

                // Bottom face
                //------------------------------------------------------------------
                if (bot) {
                    // Left line
                    if (left) {
                        rlVertex3f(x - width/2, y - height/2, z + length/2);  // Top left front
                        rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top left back
                    }

                    // Right line
                    if (right) {
                        rlVertex3f(x + width/2, y - height/2, z + length/2);  // Top right front
                        rlVertex3f(x + width/2, y - height/2, z - length/2);  // Top right back
                    }
                }
            rlEnd();*/
        rlPopMatrix();
    }
    rlEnd();
}


#endif