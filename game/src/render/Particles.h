#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"
#include "rlgl.h"

#include "./camera/camera.h"
#include "../simulation/Simulation.h"
#include "constants.h"

#include <iostream>


constexpr float width  = 1.0f;
constexpr float height = 1.0f;
constexpr float length = 1.0f;


Model getMeshFromSim(Simulation &sim) {

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texcoords;

    auto pushVertices = [&vertices, &normals, &texcoords](float x, float y, float z) {
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
        normals.push_back(0);
        normals.push_back(1);
        normals.push_back(0);
        texcoords.push_back(1);
        texcoords.push_back(0);
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

        
        // Convert world position vector to quaternion
        /*Quaternion worldPos = { px, py, pz, 1.0f };
        worldPos = QuaternionTransform(worldPos, godMatrix);
        Vector3 ndcPos = { worldPos.x/worldPos.w, -worldPos.y/worldPos.w, worldPos.z/worldPos.w };
        Vector2 val = { (ndcPos.x + 1.0f)/2.0f*(float)width, (ndcPos.y + 1.0f)/2.0f*(float)height };

        // auto val = GetWorldToScreen(Vector3{(float)px, (float)py, (float)pz}, camera);

        if (val.x < -10 || val.y < -10 || val.x > GetScreenWidth() +  10 || val.y > GetScreenHeight() + 10)
            continue;*/


        bool top = sim.pmap[pz][py + 1][px] == 0;
        bool bot = sim.pmap[pz][py - 1][px] == 0;
        bool back = sim.pmap[pz - 1][py][px] == 0;
        bool front = sim.pmap[pz + 1][py][px] == 0;
        bool left = sim.pmap[pz][py][px - 1] == 0;
        bool right = sim.pmap[pz][py][px + 1] == 0;

            // Front face
            if (front) {
                pushVertices(x - width/2, y - height/2, z + length/2);  // Bottom Left
                pushVertices(x + width/2, y - height/2, z + length/2);  // Bottom Right
                pushVertices(x - width/2, y + height/2, z + length/2);  // Top Left

                pushVertices(x + width/2, y + height/2, z + length/2);  // Top Right
                pushVertices(x - width/2, y + height/2, z + length/2);  // Top Left
                pushVertices(x + width/2, y - height/2, z + length/2);  // Bottom Right
            }

            // Back face
            if (back) {
                pushVertices(x - width/2, y - height/2, z - length/2);  // Bottom Left
                pushVertices(x - width/2, y + height/2, z - length/2);  // Top Left
                pushVertices(x + width/2, y - height/2, z - length/2);  // Bottom Right

                pushVertices(x + width/2, y + height/2, z - length/2);  // Top Right
                pushVertices(x + width/2, y - height/2, z - length/2);  // Bottom Right
                pushVertices(x - width/2, y + height/2, z - length/2);  // Top Left
            }

            // Top face
            if (top) {
                pushVertices(x - width/2, y + height/2, z - length/2);  // Top Left
                pushVertices(x - width/2, y + height/2, z + length/2);  // Bottom Left
                pushVertices(x + width/2, y + height/2, z + length/2);  // Bottom Right

                pushVertices(x + width/2, y + height/2, z - length/2);  // Top Right
                pushVertices(x - width/2, y + height/2, z - length/2);  // Top Left
                pushVertices(x + width/2, y + height/2, z + length/2);  // Bottom Right
            }

            // Bottom face
            if (bot) {
                pushVertices(x - width/2, y - height/2, z - length/2);  // Top Left
                pushVertices(x + width/2, y - height/2, z + length/2);  // Bottom Right
                pushVertices(x - width/2, y - height/2, z + length/2);  // Bottom Left

                pushVertices(x + width/2, y - height/2, z - length/2);  // Top Right
                pushVertices(x + width/2, y - height/2, z + length/2);  // Bottom Right
                pushVertices(x - width/2, y - height/2, z - length/2);  // Top Left
            }

            // Right face
            if (right) {
                pushVertices(x + width/2, y - height/2, z - length/2);  // Bottom Right
                pushVertices(x + width/2, y + height/2, z - length/2);  // Top Right
                pushVertices(x + width/2, y + height/2, z + length/2);  // Top Left

                pushVertices(x + width/2, y - height/2, z + length/2);  // Bottom Left
                pushVertices(x + width/2, y - height/2, z - length/2);  // Bottom Right
                pushVertices(x + width/2, y + height/2, z + length/2);  // Top Left
            }

            // Left face
            if (left) {
                pushVertices(x - width/2, y - height/2, z - length/2);  // Bottom Right
                pushVertices(x - width/2, y + height/2, z + length/2);  // Top Left
                pushVertices(x - width/2, y + height/2, z - length/2);  // Top Right

                pushVertices(x - width/2, y - height/2, z + length/2);  // Bottom Left
                pushVertices(x - width/2, y + height/2, z + length/2);  // Top Left
                pushVertices(x - width/2, y - height/2, z - length/2);  // Bottom Right
            }
    }
    SetTraceLogLevel(LOG_ERROR);

    Mesh mesh = { 0 };
    mesh.triangleCount = vertices.size() / 3;
    mesh.vertexCount = vertices.size();

    mesh.vertices = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));    // 3 vertices, 3 coordinates each (x, y, z)
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount*2*sizeof(float));   // 3 vertices, 2 coordinates each (x, y)
    mesh.normals = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));     // 3 vertices, 3 coordinates each (x, y, z)


    std::copy(vertices.begin(), vertices.end(), &mesh.vertices[0]);
    std::copy(texcoords.begin(), texcoords.end(), &mesh.texcoords[0]);
    std::copy(normals.begin(), normals.end(), &mesh.normals[0]);



    // Upload mesh data from CPU (RAM) to GPU (VRAM) memory
    UploadMesh(&mesh, false);

    auto mesh2 = LoadModelFromMesh(mesh);
    Image checked = GenImageChecked(2, 2, 1, 1, RED, GREEN);
    Texture2D texture = LoadTextureFromImage(checked);
    UnloadImage(checked);
    mesh2.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    return mesh2;
}

void DrawCubeParticle(Simulation &sim, RenderCamera &camera, const Color color, const Color lineColor) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    rlBegin(RL_TRIANGLES);

    //auto mesh = getMeshFromSim(sim);
    //DrawModel(mesh, Vector3{0.0f, 0.0f, 0.0f}, 1.0f, WHITE);

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