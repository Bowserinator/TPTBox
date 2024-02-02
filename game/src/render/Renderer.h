#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "rlgl.h"
#include <glad.h>

class Simulation;
class RenderCamera;
class Renderer {
public:
    Renderer(Simulation * sim, RenderCamera * cam): sim(sim), cam(cam) {}
    ~Renderer();

    void init(); // Call after openGL context has been initialized
    void update_colors_and_lod();
    void draw();
private:
    Simulation * sim;
    RenderCamera * cam;

    Shader part_shader;
    int part_shader_res_loc,
        part_shader_uv1_loc,
        part_shader_uv2_loc,
        part_shader_camera_pos_loc,
        part_shader_camera_dir_loc;

    unsigned int ssbo_colors, ssbo_lod, ubo_constants;
    struct FragShaderConstants {
        float SIM_RES[3];
        int32_t MAX_RAY_STEPS;
        int32_t NUM_LEVELS;
        float FOV_DIV2;
        uint32_t DEBUG_CASTS;
        uint32_t DEBUG_NORMALS;
        uint32_t LAYER_OFFSETS[6];
        int32_t MOD_MASK;
        int32_t X_BLOCKS;
        int32_t Y_BLOCKS;

        uint32_t MORTON_X_SHIFTS[256];
        uint32_t MORTON_Y_SHIFTS[256];
        uint32_t MORTON_Z_SHIFTS[256];
    };
};

#endif