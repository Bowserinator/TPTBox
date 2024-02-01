#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "rlgl.h"
#include <glad.h>

class Simulation;
class RenderCamera;
class Renderer {
public:
    Renderer() {}
    ~Renderer() {
        UnloadShader(part_shader);
    }

    void init(); // Call after openGL context has been initialized
    void update_texture(Simulation * sim, RenderCamera * cam);
    void draw(Simulation * sim, RenderCamera * cam);
private:
    Shader part_shader;
    int part_shader_res_loc,
        part_shader_sim_res_loc,
        part_shader_uv1_loc,
        part_shader_uv2_loc,
        part_shader_camera_pos_loc,
        part_shader_camera_dir_loc;

    int ssbo_colors, ssbo_lod; // TODO abstract to compute shader
};

#endif