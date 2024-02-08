#include "Renderer.h"
#include "../simulation/Simulation.h"
#include "../simulation/ElementClasses.h"
#include "camera/camera.h"
#include "constants.h"

#include "../util/math.h"
#include "../util/graphics.h"
#include "../util/morton.h"
#include "../util/types/ubo.h"

#include "rlgl.h"
#include "stdint.h"
#include <cstring>

Renderer::~Renderer() {
    UnloadShader(part_shader);
    UnloadRenderTexture(blur1_tex);
    UnloadRenderTexture(blur2_tex);
    UnloadRenderTexture(blur_tmp_tex);
    glDeleteBuffers(BUFFER_COUNT, ssbo_colors);
    glDeleteBuffers(BUFFER_COUNT, ssbo_flags);
    glDeleteBuffers(BUFFER_COUNT, ssbo_lod);

    glDeleteBuffers(1, &ubo_constants);
    glDeleteBuffers(1, &ubo_settings);
    glDeleteTextures(BUFFER_COUNT, ao_tex);
    glDeleteTextures(BUFFER_COUNT, shadow_tex);
    delete[] ao_data;
}

void Renderer::init() {
    part_shader = LoadShader("resources/shaders/base.vs", "resources/shaders/part.fs");
    post_shader = LoadShader("resources/shaders/base.vs", "resources/shaders/post.fs");
    blur_shader = LoadShader("resources/shaders/base.vs", "resources/shaders/blur.fs");

    ao_data = new uint8_t[sim->graphics.ao_blocks.size()];
    base_tex = MultiTexture(GetScreenWidth() / DOWNSCALE_RATIO, GetScreenHeight() / DOWNSCALE_RATIO);

    const unsigned int blur_width = GetScreenWidth() / BLUR_DOWNSCALE_RATIO;
    const unsigned int blur_height = GetScreenHeight() / BLUR_DOWNSCALE_RATIO;
    blur1_tex = LoadRenderTexture(blur_width, blur_height);
    blur2_tex = LoadRenderTexture(blur_width, blur_height);
    blur_tmp_tex = LoadRenderTexture(blur_width, blur_height);

    rlEnableShader(part_shader.id);
        rlSetUniformSampler(rlGetLocationUniform(part_shader.id, "FragColor"), 0);
        rlSetUniformSampler(rlGetLocationUniform(part_shader.id, "FragGlowOnly"), 1);
        rlSetUniformSampler(rlGetLocationUniform(part_shader.id, "FragBlurOnly"), 2);
    rlDisableShader();

    // Uniform values that may change per frame
    part_shader_res_loc = GetShaderLocation(part_shader, "resolution");
    part_shader_camera_pos_loc = GetShaderLocation(part_shader, "cameraPos");
    part_shader_camera_dir_loc = GetShaderLocation(part_shader, "cameraDir");
    part_shader_uv1_loc = GetShaderLocation(part_shader, "uv1");
    part_shader_uv2_loc = GetShaderLocation(part_shader, "uv2");

    post_shader_base_texture_loc = GetShaderLocation(post_shader, "baseTexture");
    post_shader_glow_texture_loc = GetShaderLocation(post_shader, "glowTexture");
    post_shader_blur_texture_loc = GetShaderLocation(post_shader, "blurTexture");
    post_shader_depth_texture_loc = GetShaderLocation(post_shader, "depthTexture");
    post_shader_res_loc = GetShaderLocation(post_shader, "resolution");

    blur_shader_base_texture_loc = GetShaderLocation(blur_shader, "baseTexture");
    blur_shader_res_loc = GetShaderLocation(blur_shader, "resolution");
    blur_shader_dir_loc = GetShaderLocation(blur_shader, "direction");

    // SSBOs for color & octree LOD data
    for (auto i = 0; i < BUFFER_COUNT; i++) {
        ssbo_colors[i] = rlLoadShaderBuffer(XRES * YRES * ZRES * sizeof(uint32_t), NULL, RL_DYNAMIC_COPY);
        ssbo_flags[i]  = rlLoadShaderBuffer(XRES * YRES * ZRES * sizeof(uint8_t), NULL, RL_DYNAMIC_COPY);
        ssbo_lod[i]    = rlLoadShaderBuffer(sizeof(uint8_t) *  OctreeBlockMetadata::size * X_BLOCKS * Y_BLOCKS * Z_BLOCKS,
            NULL, RL_DYNAMIC_COPY);
    }

    // Ambient occlusion texture, uses texture for free linear filtering
    glGenTextures(BUFFER_COUNT, ao_tex);
    for (auto i = 0; i < BUFFER_COUNT; i++) {
        glBindTexture(GL_TEXTURE_3D, ao_tex[i]);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, AO_X_BLOCKS, AO_Y_BLOCKS, AO_Z_BLOCKS, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    }

    // Shadow texture
    glGenTextures(BUFFER_COUNT, shadow_tex);
    for (auto i = 0; i < BUFFER_COUNT; i++) {
        glBindTexture(GL_TEXTURE_2D, shadow_tex[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SHADOW_MAP_X, SHADOW_MAP_Y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    }

    // Uniform constants
    {
    #ifdef DEBUG
        if (cam->camera.fovy == 0.0)
            throw std::invalid_argument("Render camera fov should not be 0 (renderer should be initialized AFTER the camera)");
    #endif
        glGenBuffers(1, &ubo_constants);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_constants);
        UBOWriter constants_writer(part_shader.id, ubo_constants, "Constants");
        glBufferData(GL_UNIFORM_BUFFER, constants_writer.size(), NULL, GL_STATIC_DRAW);

        float SIMRES[] = { (float)XRES, (float)YRES, (float)ZRES };
        int32_t OCTTREE_BLOCK_DIMS[3] = { X_BLOCKS, Y_BLOCKS, Z_BLOCKS };
        int32_t AO_BLOCK_DIMS[3] = { AO_X_BLOCKS, AO_Y_BLOCKS, AO_Z_BLOCKS };

        constants_writer.write_member("SIMRES", SIMRES);
        constants_writer.write_member("NUM_LEVELS", OCTREE_BLOCK_DEPTH);
        constants_writer.write_member("FOV_DIV2", cam->camera.fovy * DEG2RAD / 2.0f);
        constants_writer.write_member("MOD_MASK", (1 << OCTREE_BLOCK_DEPTH) - 1);
        constants_writer.write_member("AO_BLOCK_SIZE", AO_BLOCK_SIZE);
        constants_writer.write_member("OCTTREE_BLOCK_DIMS", OCTTREE_BLOCK_DIMS);
        constants_writer.write_member("AO_BLOCK_DIMS", AO_BLOCK_DIMS);

        constants_writer.write_member("LAYER_OFFSETS", &OctreeBlockMetadata::layer_offsets[0], OCTREE_BLOCK_DEPTH * sizeof(unsigned int));
        constants_writer.write_member("MORTON_X_SHIFTS", Morton::X_SHIFTS);
        constants_writer.write_member("MORTON_Y_SHIFTS", Morton::Y_SHIFTS);
        constants_writer.write_member("MORTON_Z_SHIFTS", Morton::Z_SHIFTS);
        constants_writer.upload();
    }

    // UBO: settings
    {
        glGenBuffers(1, &ubo_settings);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_settings);
        UBOWriter settings_writer(part_shader.id, ubo_settings, "Settings");
        glBufferData(GL_UNIFORM_BUFFER, settings_writer.size(), NULL, GL_STATIC_DRAW);

        float BG_COLOR[] = { BACKGROUND_COLOR.r / 255.0f, BACKGROUND_COLOR.g / 255.0f, BACKGROUND_COLOR.b / 255.0f };
        float SH_COLOR[] = { SHADOW_COLOR.r / 255.0f, SHADOW_COLOR.g / 255.0f, SHADOW_COLOR.b / 255.0f };
        settings_writer.write_member("MAX_RAY_STEPS", 256 * 2);
        settings_writer.write_member("DEBUG_MODE", FragDebugMode::NODEBUG);
        settings_writer.write_member("AO_STRENGTH", 0.6f);
        settings_writer.write_member("BACKGROUND_COLOR", BG_COLOR);
        settings_writer.write_member("SHADOW_STRENGTH", 0.35f);
        settings_writer.write_member("SHADOW_COLOR", SH_COLOR);

        settings_writer.write_member("ENABLE_TRANSPARENCY", 1);
        settings_writer.write_member("ENABLE_REFLECTION", 1);
        settings_writer.write_member("ENABLE_REFRACTION", 1);
        settings_writer.write_member("ENABLE_BLUR", 1);
        settings_writer.write_member("ENABLE_GLOW", 1);
        settings_writer.write_member("ENABLE_AO", 1);

        settings_writer.upload();
    }
}

void Renderer::update_colors_and_lod() {
    const unsigned int ssbo_idx = (frame_count + 1) % BUFFER_COUNT;
    const unsigned int ssbo_bit = 1 << ssbo_idx;

    for (std::size_t i = 0; i < COLOR_DATA_CHUNK_COUNT; i++) {
        if (sim->graphics.color_data_modified[i] & ssbo_bit) {
            // Since the chunks might overestimate actual color count
            // on last one take # of colors - last chunk boundary
            const auto chunk_len = (i == COLOR_DATA_CHUNK_COUNT - 1) ?
                sim->graphics.color_data.size() - i * COLOR_DATA_CHUNK_SIZE :
                COLOR_DATA_CHUNK_SIZE;

            rlUpdateShaderBuffer(ssbo_colors[ssbo_idx],
                &sim->graphics.color_data[i * COLOR_DATA_CHUNK_SIZE],
                chunk_len * sizeof(uint32_t),
                i * COLOR_DATA_CHUNK_SIZE * sizeof(uint32_t));
            rlUpdateShaderBuffer(ssbo_flags[ssbo_idx],
                &sim->graphics.color_flags[i * COLOR_DATA_CHUNK_SIZE],
                chunk_len * sizeof(uint8_t),
                i * COLOR_DATA_CHUNK_SIZE * sizeof(uint8_t));

            sim->graphics.color_data_modified[i] &= ~ssbo_bit;
        }
    }

    for (std::size_t i = 0; i < sim->graphics.octree_blocks.size(); i++) {
        if (sim->graphics.octree_blocks[i].modified & ssbo_bit) {
            // We do not upload the whole octree here, we upload all layers except
            // the last layer, since the last layer only stores info about the 1x1x1 voxel
            // data which we already have in the form of color_data
            rlUpdateShaderBuffer(ssbo_lod[ssbo_idx], sim->graphics.octree_blocks[i].data,
                sizeof(uint8_t) * OctreeBlockMetadata::layer_offsets[OCTREE_BLOCK_DEPTH - 1],
                i * sizeof(uint8_t) * OctreeBlockMetadata::layer_offsets[OCTREE_BLOCK_DEPTH - 1]);
            sim->graphics.octree_blocks[i].modified &= ~ssbo_bit;
        }
    }

    glBindTexture(GL_TEXTURE_3D, ao_tex[ssbo_idx]);
    constexpr unsigned int AO_VOLUME = AO_BLOCK_SIZE * AO_BLOCK_SIZE * AO_BLOCK_SIZE;
    #pragma omp simd
    for (int i = 0; i < sim->graphics.ao_blocks.size(); i++)
        ao_data[i] = 255 * sim->graphics.ao_blocks[i] / AO_VOLUME;
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, AO_X_BLOCKS, AO_Y_BLOCKS, AO_Z_BLOCKS, GL_RED, GL_UNSIGNED_BYTE, ao_data);

    glBindTexture(GL_TEXTURE_2D, shadow_tex[ssbo_idx]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SHADOW_MAP_X, SHADOW_MAP_Y, GL_RED, GL_UNSIGNED_BYTE, sim->graphics.shadow_map);
}

void Renderer::draw_octree_debug() {
    for (std::size_t i = 0; i < sim->graphics.octree_blocks.size(); i++) {
        if (!sim->graphics.octree_blocks[i].data[0]) continue;
        int blockX = i % X_BLOCKS;
        int blockY = (i / X_BLOCKS) % Y_BLOCKS;
        int blockZ = (i / X_BLOCKS / Y_BLOCKS);

        for (int layer = OCTREE_BLOCK_DEPTH - 3; layer <= OCTREE_BLOCK_DEPTH; layer++) {
        for (int dz = 0; dz < (OCTREE_BLOCK_DIM >> layer); dz++) {
        for (int dy = 0; dy < (OCTREE_BLOCK_DIM >> layer); dy++) {
        for (int dx = 0; dx < (OCTREE_BLOCK_DIM >> layer); dx++) {
            unsigned int morton = util::morton_decode8(dx, dy, dz);

            if (sim->graphics.octree_blocks[i].data[morton + OctreeBlockMetadata::layer_offsets[OCTREE_BLOCK_DEPTH - layer]] != 0) {
                float trueX = static_cast<float>((dx << layer) + blockX * OCTREE_BLOCK_DIM);
                float trueY = static_cast<float>((dy << layer) + blockY * OCTREE_BLOCK_DIM);
                float trueZ = static_cast<float>((dz << layer) + blockZ * OCTREE_BLOCK_DIM);
                int size = 1 << layer;

                DrawCubeWires(
                    Vector3{ trueX + size / 2, trueY + size / 2, trueZ + size / 2 },
                    size, size, size, Color{255, 255, 255, 50}
                );
            }
        }}}}
    }
}

void Renderer::draw() {
    update_colors_and_lod();
    // draw_octree_debug();

    const unsigned int ssbo_idx = sim->frame_count % BUFFER_COUNT;
    rlBindShaderBuffer(ssbo_colors[ssbo_idx], 0);
    rlBindShaderBuffer(ssbo_flags[ssbo_idx], 1);
    rlBindShaderBuffer(ssbo_lod[ssbo_idx], 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_3D, ao_tex[ssbo_idx]);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadow_tex[ssbo_idx]);

    glBindBufferBase(GL_UNIFORM_BUFFER, 5, ubo_constants);
    glBindBufferBase(GL_UNIFORM_BUFFER, 6, ubo_settings);


#pragma region uniforms
    const Vector2 resolution{ (float)GetScreenWidth(), (float)GetScreenHeight() };
    const Vector2 virtual_resolution{ (float)GetScreenWidth() / DOWNSCALE_RATIO, (float)GetScreenHeight() / DOWNSCALE_RATIO };
    const Vector2 blur_resolution{ (float)GetScreenWidth() / BLUR_DOWNSCALE_RATIO, (float)GetScreenHeight() / BLUR_DOWNSCALE_RATIO };
    
    // Inverse camera rotation matrix
    auto transform_mat = MatrixLookAt(cam->camera.position, cam->camera.target, cam->camera.up);
    util::reduce_to_rotation(transform_mat);
    const auto transform_matT = MatrixTranspose(transform_mat);

    const float aspect_ratio = resolution.x / resolution.y;
    const auto look_ray = Vector3Normalize(cam->camera.target - cam->camera.position);

    // Actual 3D vectors in world space that correspond to screen RIGHT (uv1) and screen UP (uv2)
    const Vector3 uv1 = Vector3Transform(Vector3{1.0, 0.0, 0.0} * aspect_ratio, transform_matT);
    const Vector3 uv2 = Vector3Transform(Vector3{0.0, 1.0, 0.0}, transform_matT);

    // Fullscreen triangle vertices
    const Vector3 cent = cam->camera.position + look_ray; // Center pos of triangle
    const Vector3 fullt1 = cent - 2.0f * uv1 + uv2;
    const Vector3 fullt2 = cent + uv1 - 2.0 * uv2;
    const Vector3 fullt3 = cent + uv1 + uv2;
#pragma endregion uniforms


    // First render everything to FBO
    // which contains multiple textures for glow, blur, base, depth, etc...
    rlEnableFramebuffer(base_tex.frameBuffer);
    rlClearScreenBuffers();
    rlDisableColorBlend();

    BeginMode3D(cam->camera);
    BeginShaderMode(part_shader);

        ClearBackground(Color{0, 0, 0, 0}); // Semi-transparent background

        util::set_shader_value(part_shader, part_shader_res_loc, virtual_resolution);
        util::set_shader_value(part_shader, part_shader_camera_pos_loc, cam->camera.position);
        util::set_shader_value(part_shader, part_shader_camera_dir_loc, look_ray);
        util::set_shader_value(part_shader, part_shader_uv1_loc, uv1);
        util::set_shader_value(part_shader, part_shader_uv2_loc, uv2);

        // Draw fullscreen triangle, the overdraw has 0 effect on performance
        DrawTriangle3D(fullt1, fullt2, fullt3, WHITE);

    EndShaderMode();
    EndMode3D();

    rlDisableFramebuffer();
    rlEnableColorBlend();

    // TODO: time - 0.2ms
    auto start = GetTime();
    _blur_render_texture(base_tex.glowOnlyTexture, blur_resolution, blur1_tex);
    _blur_render_texture(base_tex.blurOnlyTexture, blur_resolution, blur2_tex);
    auto end = GetTime();
    // std::cout << (end -start )<<'\n';

    // Render the above textures with a post-processing shader for compositing
    BeginMode3D(cam->camera);
    rlDisableColorBlend();
    BeginShaderMode(post_shader);

        rlEnableShader(post_shader.id);
        rlSetUniformSampler(post_shader_base_texture_loc, base_tex.colorTexture);
        rlSetUniformSampler(post_shader_glow_texture_loc, blur1_tex.texture.id);
        rlSetUniformSampler(post_shader_blur_texture_loc, blur2_tex.texture.id);
        rlSetUniformSampler(post_shader_depth_texture_loc, base_tex.depthTexture);
        util::set_shader_value(post_shader, post_shader_res_loc, resolution);

        DrawTriangle3D(fullt1, fullt2, fullt3, WHITE);
        glBindTexture(GL_TEXTURE_2D, 0);

    EndShaderMode();
    rlEnableColorBlend();
    EndMode3D();

    frame_count++;
}


/**
 * @brief Blur a render texture
 * 
 * @param textureInId ID of the input texture, if you use Texture2D pass texture.id
 * @param resolution Resolution of the texture
 * @param blur_tex Output texture blurred image is written to
 */
void Renderer::_blur_render_texture(unsigned int textureInId, const Vector2 resolution, RenderTexture2D &blur_tex) {
    BeginShaderMode(blur_shader);

    // Split into 2 passes: horizontal and vertical
    for (int i = 0; i < 2; i++) {
        BeginTextureMode(i == 0 ? blur_tmp_tex : blur_tex);
            ClearBackground(Color{0, 0, 0, 0});
            util::set_shader_value(blur_shader, blur_shader_res_loc, resolution);
            util::set_shader_value(blur_shader, blur_shader_dir_loc, Vector2{ float(i), float(1 - i) });
            rlSetUniformSampler(blur_shader_base_texture_loc, i == 0 ? textureInId : blur_tmp_tex.texture.id);
            DrawRectangle(0, 0, resolution.x, resolution.y, WHITE);
        EndTextureMode();
    }

    EndShaderMode();
}
