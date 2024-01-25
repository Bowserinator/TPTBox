#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "rlgl.h"
#include <glad.h>

#include <iostream>

class Simulation;
class RenderCamera;

class Renderer {
public:
    Renderer() {}
    ~Renderer() {
        UnloadShader(part_shader);
        glDeleteTextures(1, &color_tex);
        for (int i = 0; i < 6; i++) // TODO constant
            glDeleteTextures(1, &lod[i]);
    }

    void init(); // Call after openGL context has been initialized
    void draw(Simulation * sim, RenderCamera * cam);
private:
    GLuint color_tex; // 6 mipmap reductions
    unsigned int lod[6];

    Shader part_shader;
    int part_shader_res_loc,
        part_shader_sim_res_loc,
        part_shader_uv1_loc,
        part_shader_uv2_loc,
        part_shader_camera_pos_loc,
        part_shader_camera_dir_loc;
};

#endif