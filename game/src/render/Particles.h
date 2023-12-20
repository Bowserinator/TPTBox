#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"
#include "rlgl.h"
#include "../simulation/Simulation.h"

void DrawCubeParticle(Simulation &sim, const Color color, const Color lineColor) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    constexpr float width  = 1.0f;
    constexpr float height = 1.0f;
    constexpr float length = 1.0f;
    rlEnableBackfaceCulling();

    for (int i = 0; i < sim.maxId; i++) { // TODO: size_t doesn't exist??
        const auto &part = sim.parts[i];
        if (!part.id) continue;

        int px = (int)(sim.parts[i].x + 0.5f);
        int py = (int)(sim.parts[i].y + 0.5f);
        int pz = (int)(sim.parts[i].z + 0.5f);

        bool top = sim.pmap[pz][py + 1][px] == 0;
        bool bot = sim.pmap[pz][py - 1][px] == 0;
        bool back = sim.pmap[pz - 1][py][px] == 0;
        bool front = sim.pmap[pz + 1][py][px] == 0;
        bool left = sim.pmap[pz][py][px - 1] == 0;
        bool right = sim.pmap[pz][py][px + 1] == 0;

        rlPushMatrix();
            // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
            rlTranslatef(px, py, pz);

            rlBegin(RL_TRIANGLES);
                rlColor4ub(color.r, color.g, color.b, color.a);

                // Front face
                if (front) {
                    rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left
                    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
                    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left

                    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Right
                    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
                    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
                }

                // Back face
                if (back) {
                    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Left
                    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
                    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right

                    rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
                    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
                    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
                }

                // Top face
                if (top) {
                    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
                    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Bottom Left
                    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Bottom Right

                    rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
                    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
                    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Bottom Right
                }

                // Bottom face
                if (bot) {
                    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Left
                    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
                    rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left

                    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Top Right
                    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
                    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Left
                }

                // Right face
                if (right) {
                    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
                    rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
                    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Left

                    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Left
                    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
                    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Left
                }

                // Left face
                if (left) {
                    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Right
                    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
                    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Right

                    rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left
                    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
                    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Right
                }
            rlEnd();

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
            rlEnd();
        rlPopMatrix();
    }
}


#endif