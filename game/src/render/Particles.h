#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"
#include "rlgl.h"

#include "./camera/camera.h"
#include "../simulation/Simulation.h"
#include "constants.h"

#include <iostream>

#include <glad.h>


constexpr float width  = 1.0f;
constexpr float height = 1.0f;
constexpr float length = 1.0f;

static Mesh mesh = { 0 };
static bool found = false;
static std::vector<float> vertices;
static std::vector<float> texcoords;

void getMeshFromSim(Simulation &sim, RenderCamera & camera, Texture2D &texture) {
    vertices.clear();
    texcoords.clear();

    constexpr int BUFF_SIZE = sizeof(float) * 10000000; // 10^7
    if (!found) {
        SetTraceLogLevel(LOG_ERROR);
    
        mesh.vertices = (float *)MemAlloc(BUFF_SIZE * 3);    // 3 vertices, 3 coordinates each (x, y, z)
        mesh.texcoords = (float *)MemAlloc(BUFF_SIZE * 2);   // 3 vertices, 2 coordinates each (x, y)
        mesh.colors = (unsigned char*)MemAlloc(BUFF_SIZE * 4);
    }
    int vertex_count = 0;
    int color_count = 0;


    auto pushVertices = [&color_count, &vertex_count](unsigned char red, float x, float y, float z) {
        //vertices.push_back(x);
        //vertices.push_back(y);
        //vertices.push_back(z);
        mesh.vertices[vertex_count] = x;
        mesh.vertices[vertex_count + 1] = y;
        mesh.vertices[vertex_count + 2] = z;
        vertex_count += 3;
        //texcoords.push_back(1);
        //texcoords.push_back(0);
        mesh.colors[color_count] = red;
        mesh.colors[color_count + 1] = 0;
        mesh.colors[color_count + 2] = 0xAA;
        mesh.colors[color_count + 3] = 0xFF;
        color_count += 4;
    };

    for (int i = 0; i < sim.maxId; i++) { // TODO: size_t doesn't exist??
        const auto &part = sim.parts[i];
        if (!part.id) continue;

        int px = (int)(sim.parts[i].x + 0.5f);
        int py = (int)(sim.parts[i].y + 0.5f);
        int pz = (int)(sim.parts[i].z + 0.5f);

        float x = px;
        float y = py;
        float z = pz;

        unsigned char red = 0xFF;
       //if (camera.sphereOutsideFrustum(px, py, pz, DIS_UNIT_CUBE_CENTER_TO_CORNER))
       //     continue;


        bool top = sim.pmap[pz][py + 1][px] == 0;
        bool bot = sim.pmap[pz][py - 1][px] == 0;
        bool back = sim.pmap[pz - 1][py][px] == 0;
        bool front = sim.pmap[pz + 1][py][px] == 0;
        bool left = sim.pmap[pz][py][px - 1] == 0;
        bool right = sim.pmap[pz][py][px + 1] == 0;

        // Front face
        if (front) {
            red = 0xAA;
            pushVertices(red, x - width/2, y - height/2, z + length/2);  // Bottom Left
            pushVertices(red, x + width/2, y - height/2, z + length/2);  // Bottom Right
            pushVertices(red, x - width/2, y + height/2, z + length/2);  // Top Left

            pushVertices(red, x + width/2, y + height/2, z + length/2);  // Top Right
            pushVertices(red, x - width/2, y + height/2, z + length/2);  // Top Left
            pushVertices(red, x + width/2, y - height/2, z + length/2);  // Bottom Right
        }

        // Back face
        if (back) {
            red = 0xAA;
            pushVertices(red, x - width/2, y - height/2, z - length/2);  // Bottom Left
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Left
            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Bottom Right

            pushVertices(red, x + width/2, y + height/2, z - length/2);  // Top Right
            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Bottom Right
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Left
        }

        // Top face
        if (top) {
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Left
            pushVertices(red, x - width/2, y + height/2, z + length/2);  // Bottom Left
            pushVertices(red, x + width/2, y + height/2, z + length/2);  // Bottom Right

            pushVertices(red, x + width/2, y + height/2, z - length/2);  // Top Right
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Left
            pushVertices(red, x + width/2, y + height/2, z + length/2);  // Bottom Right
        }

        // Bottom face
        if (bot) {
            pushVertices(red, x - width/2, y - height/2, z - length/2);  // Top Left
            pushVertices(red, x + width/2, y - height/2, z + length/2);  // Bottom Right
            pushVertices(red, x - width/2, y - height/2, z + length/2);  // Bottom Left

            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Top Right
            pushVertices(red, x + width/2, y - height/2, z + length/2);  // Bottom Right
            pushVertices(red, x - width/2, y - height/2, z - length/2);  // Top Left
        }

        // Right face
        if (right) {
            red = 0xCC;
            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Bottom Right
            pushVertices(red, x + width/2, y + height/2, z - length/2);  // Top Right
            pushVertices(red, x + width/2, y + height/2, z + length/2);  // Top Left

            pushVertices(red, x + width/2, y - height/2, z + length/2);  // Bottom Left
            pushVertices(red, x + width/2, y - height/2, z - length/2);  // Bottom Right
            pushVertices(red, x + width/2, y + height/2, z + length/2);  // Top Left
        }

        // Left face
        if (left) {
            red = 0xCC;
            pushVertices(red, x - width/2, y - height/2, z - length/2);  // Bottom Right
            pushVertices(red, x - width/2, y + height/2, z + length/2);  // Top Left
            pushVertices(red, x - width/2, y + height/2, z - length/2);  // Top Right

            pushVertices(red, x - width/2, y - height/2, z + length/2);  // Bottom Left
            pushVertices(red, x - width/2, y + height/2, z + length/2);  // Top Left
            pushVertices(red, x - width/2, y - height/2, z - length/2);  // Bottom Right
        }
    }

    mesh.triangleCount = vertex_count / 3;
    mesh.vertexCount = vertex_count;


    // if (found) {
    //     MemFree(mesh.vertices);
    //     MemFree(mesh.texcoords);
    //     MemFree(mesh.normals);
    // }

    
    if (!found) {
        found = true;
        UploadMesh(&mesh, true);
    } else {
        UpdateMeshBuffer(mesh, 0, &mesh.vertices[0], sizeof(float) * vertex_count, 0);
        UpdateMeshBuffer(mesh, 3, &mesh.colors[0], sizeof(unsigned char) * color_count, 0);
        // UpdateMeshBuffer(mesh, 1, &texcoords[0], sizeof(float) * texcoords.size(), 0);
    }

}

void DrawCubeParticle(Simulation &sim, RenderCamera &camera, const Color color, const Color lineColor, Texture2D &texture) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    getMeshFromSim(sim, camera, texture);
    DrawMesh(mesh, LoadMaterialDefault(), MatrixIdentity()); 
    //auto mesh2 = getMeshFromSim(sim, camera, texture);
    //DrawModelWires(mesh2, Vector3{0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
    // UnloadModel(mesh2);
    return;


    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < sim.maxId; i++) { // TODO: size_t doesn't exist??
        const auto &part = sim.parts[i];
        if (!part.id) continue;

        int px = (int)(sim.parts[i].x + 0.5f);
        int py = (int)(sim.parts[i].y + 0.5f);
        int pz = (int)(sim.parts[i].z + 0.5f);

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