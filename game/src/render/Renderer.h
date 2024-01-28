#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "rlgl.h"
#include <glad.h>

#include <iostream>

constexpr int LOD_LEVELS = 6;

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
    void update_texture(Simulation * sim, RenderCamera * cam);
    void draw(Simulation * sim, RenderCamera * cam);
private:
    void create_texture_data_arrays();

    GLuint color_tex;
    unsigned int lod[LOD_LEVELS];

    uint32_t * color_data;
    uint8_t ** lod_data;

    Shader part_shader;
    int part_shader_res_loc,
        part_shader_sim_res_loc,
        part_shader_uv1_loc,
        part_shader_uv2_loc,
        part_shader_camera_pos_loc,
        part_shader_camera_dir_loc;
};

#endif