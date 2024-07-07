#ifndef RENDER_RENDERER_H_
#define RENDER_RENDERER_H_

#include "raylib.h"
#include "rlgl.h"
#include <glad.h>

#include "types/multitexture.h"
#include "types/octree.h"
#include "../util/types/persistent_buffer.h"
#include "constants.h"

constexpr unsigned int BUFFER_COUNT = 3; // Must be < 8 because modified bitset is 1 byte
constexpr std::size_t HEAT_GRADIENT_STEPS = 1024;

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
    void set_grid_size(float size);

    MultiTexture& get_base_tex() { return base_tex; }
    RenderCamera * get_cam() { return cam; }

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
    UBOWriter * air_constants_writer = nullptr;
    Vector2 preFullscreenWindowRes;

    Shader part_shader, post_shader, blur_shader, air_shader;
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
    int air_shader_res_loc,
        air_shader_uv1_loc,
        air_shader_uv2_loc,
        air_shader_camera_pos_loc,
        air_shader_camera_dir_loc;

    GLuint ao_tex[BUFFER_COUNT], shadow_tex[BUFFER_COUNT];
    util::PersistentBuffer<BUFFER_COUNT> colorBuf;
    util::PersistentBuffer<BUFFER_COUNT> flagBuf;
    util::PersistentBuffer<BUFFER_COUNT> lodBuf;

    unsigned int ssbo_constants;
    unsigned int ubo_settings;
    unsigned int ssbo_display_mode;
    unsigned int air_ubo;
    uint8_t * ao_data;

    float downscaleRatio = 1.5f;
    float blurDownscaleRatio = 1.5f;

    RenderTexture2D blur1_tex = {0}, blur2_tex = {0}, blur_tmp_tex = {0};
    MultiTexture base_tex;
    unsigned int frame_count = 0;
    bool show_octree = false;
    bool do_blur = true;
    bool do_glow = true;
    bool do_shadows = true;
    bool do_ao = true;

    Color background_color;
    Color shadow_color;

    Shader grid_shader;
    unsigned int grid_shader_size_loc,
                 grid_shader_scale_loc;
    unsigned int grid_max_dim;
    Model grid_model;
    float grid_scale = 0.0;

    DisplayMode cached_display_mode = DisplayMode::DISPLAY_MODE_NOTHING;

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
        uint32_t HEAT_GRADIENT[HEAT_GRADIENT_STEPS];
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct SSBO_DisplayMode_t {
        uint32_t display_mode;
    };
    #pragma pack(pop)

    void _generate_render_textures();
    void _blur_render_texture(unsigned int textureInId, const Vector2 resolution, RenderTexture2D &blur_tex);
};

#endif // RENDER_RENDERER_H_
