#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "rlgl.h"
#include <glad.h>

#include "types/multitexture.h"
#include "types/octree.h"
#include "../util/types/persistent_buffer.h"
#include "constants.h"

constexpr float DOWNSCALE_RATIO = 1.5f;
constexpr float BLUR_DOWNSCALE_RATIO = 1.5f;
constexpr unsigned int BUFFER_COUNT = 3; // Must be < 8 because modified bitset is 1 byte
constexpr Color BACKGROUND_COLOR{ 0, 0, 0, 255 };
constexpr Color SHADOW_COLOR{ 32, 18, 39, 255 };

class Simulation;
class RenderCamera;
class UBOWriter;
namespace settings { class Graphics; }

class Renderer {
public:
    Renderer(Simulation * sim, RenderCamera * cam): sim(sim), cam(cam), ao_data(nullptr) {}
    ~Renderer();

    void init(); // Call after openGL context has been initialized
    void update_colors_and_lod();
    void draw();
    void draw_octree_debug();
    void update_settings(settings::Graphics * settings);
    void update_grid();
    void set_grid_size(float size);

    enum class FragDebugMode: uint32_t {
        NODEBUG = 0,
        DEBUG_STEPS = 1,
        DEBUG_NORMALS = 2,
        DEBUG_AO = 3
    };
private:
    Simulation * sim;
    RenderCamera * cam;
    UBOWriter * settings_writer = nullptr;

    Shader part_shader, post_shader, blur_shader;
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
    int blur_shader_base_texture_loc,
        blur_shader_res_loc,
        blur_shader_dir_loc;

    GLuint ao_tex[BUFFER_COUNT], shadow_tex[BUFFER_COUNT];
    util::PersistentBuffer<BUFFER_COUNT> colorBuf;
    util::PersistentBuffer<BUFFER_COUNT> flagBuf;
    util::PersistentBuffer<BUFFER_COUNT> lodBuf;

    unsigned int ssbo_constants;
    unsigned int ubo_settings;
    uint8_t * ao_data;

    RenderTexture2D blur1_tex, blur2_tex, blur_tmp_tex;
    MultiTexture base_tex;
    unsigned int frame_count = 0;
    bool show_octree = false;
    bool do_blur = true;
    bool do_glow = true;
    bool do_shadows = true;
    bool do_ao = true;

    Shader grid_shader;
    RenderTexture2D grid_tex;
    unsigned int grid_shader_size_loc,
                 grid_shader_scale_loc;
    unsigned int grid_max_dim;
    Model grid_model;
    float grid_scale = 0.0;

    #pragma pack(push, 1)
    // Vec3s are vec4s and big arrays are at the end for packing purposes
    // std430 packing rules apply
    struct SSBO_Constants_t {
        float SIMRES[4];
        int32_t NUM_LEVELS;
        float FOV_DIV2;

        int32_t MOD_MASK;
        int32_t AO_BLOCK_SIZE;
        int32_t OCTTREE_BLOCK_DIMS[4];
        int32_t AO_BLOCK_DIMS[4];

        uint32_t LAYER_OFFSETS[OCTREE_BLOCK_DEPTH];
        uint32_t MORTON_X_SHIFTS[256];
        uint32_t MORTON_Y_SHIFTS[256];
        uint32_t MORTON_Z_SHIFTS[256];
    };
    #pragma pack(pop)

    void _blur_render_texture(unsigned int textureInId, const Vector2 resolution, RenderTexture2D &blur_tex);
};

#endif