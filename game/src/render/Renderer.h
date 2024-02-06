#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "rlgl.h"
#include <glad.h>

#include "types/multitexture.h"
#include "types/octree.h"

constexpr float DOWNSCALE_RATIO = 2.0f;

class Simulation;
class RenderCamera;
class Renderer {
public:
    Renderer(Simulation * sim, RenderCamera * cam): sim(sim), cam(cam), ao_data(nullptr) {}
    ~Renderer();

    void init(); // Call after openGL context has been initialized
    void update_colors_and_lod();
    void draw();
    void draw_octree_debug();
private:
    Simulation * sim;
    RenderCamera * cam;

    Shader part_shader, post_shader;
    int part_shader_res_loc,
        part_shader_uv1_loc,
        part_shader_uv2_loc,
        part_shader_camera_pos_loc,
        part_shader_camera_dir_loc;
    int post_shader_base_texture_loc,
        post_shader_glow_texture_loc,
        post_shader_blur_texture_loc,
        post_shader_depth_texture_loc,
        post_shader_res_loc;

    GLuint ao_tex;
    unsigned int ssbo_colors, ssbo_lod, ubo_constants, ubo_settings;
    uint8_t * ao_data;

    MultiTexture base_tex;

    enum class FragDebugMode: uint32_t {
        NODEBUG = 0,
        DEBUG_STEPS = 1,
        DEBUG_NORMALS = 2,
        DEBUG_AO = 3
    };
};

#endif