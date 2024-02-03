#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "rlgl.h"
#include <glad.h>

#include "types/octree.h"

class Simulation;
class RenderCamera;
class Renderer {
public:
    Renderer(Simulation * sim, RenderCamera * cam): sim(sim), cam(cam) {}
    ~Renderer();

    void init(); // Call after openGL context has been initialized
    void update_colors_and_lod();
    void draw();
    void draw_octree_debug();
private:
    Simulation * sim;
    RenderCamera * cam;

    Shader part_shader;
    int part_shader_res_loc,
        part_shader_uv1_loc,
        part_shader_uv2_loc,
        part_shader_camera_pos_loc,
        part_shader_camera_dir_loc;

    unsigned int ssbo_colors, ssbo_lod, ssbo_ao, ubo_constants, ubo_settings;

    enum class FragDebugMode: uint32_t {
        NODEBUG = 0,
        DEBUG_STEPS = 1,
        DEBUG_NORMALS = 2,
        DEBUG_AO = 3
    };
};

#endif