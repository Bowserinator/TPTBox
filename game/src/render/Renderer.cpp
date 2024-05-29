#include "Renderer.h"
#include "../simulation/Simulation.h"
#include "../simulation/ElementClasses.h"
#include "camera/camera.h"
#include "constants.h"
#include "../graphics/gradient.h"
#include "../interface/settings/data/GraphicsSettingsData.h"
#include "../interface/settings/data/SettingsData.h"

#include "../util/math.h"
#include "../util/graphics.h"
#include "../util/morton.h"
#include "../util/types/ubo.h"

#include "rlgl.h"
#include "stdint.h"
#include <glad.h>

#include <array>
#include <cstring>

// Custom cube mesh generation:
// a) shows only inside faces
// b) texture UVs are cropped relative to largest dimension
Mesh GenInvertedMeshCube(const float width, const float height, const float length) {
    Mesh mesh = { 0 };
    float vertices[108]; // 3 points * 3 vertices * 12 triangles
    float texcoords[72]; // 2 points * 3 vertices * 12 triangles

    const float dims[] = { width, height, length };
    const float largestDim = std::max({ width, height, length });

    constexpr float vertices2D[] = {
        0, 0, 1, 1, 0, 1,
        0, 0, 1, 0, 1, 1,
        0, 0, 0, 1, 1, 1,
        0, 0, 1, 1, 1, 0
    };

    int i = 0;
    int k = 0;
    for (int axis = 0; axis < 3; axis++) {
        for (int j = 0; j < 36; j++) {
            // j >= 18 is when axis = 1, before axis = 0
            // Except for axis = 1 (Y) which is inverted for some reason
            float axisValue = j >= 18 ? 1 : 0;
            if (axis == 1) axisValue = 1.0f - axisValue;

            vertices[i] = (i % 3 == axis) ? axisValue : vertices2D[k % 24];
            if (i % 3 != axis) {
                texcoords[k] = vertices2D[k % 24] * dims[i % 3] / largestDim;
                // Flip y dim because render texture is flipped
                if (k % 2 == 1) texcoords[k] = 1.0f - texcoords[k];
                // Get x face to line up for some reason
                else if (k % 2 == 0 && axis == 0) texcoords[k] = 1.0f - texcoords[k];
                k++;
            }
            i++;
        }
    }

    for (std::size_t i = 0; i < sizeof(vertices) / sizeof(float); i += 3) {
        vertices[i]     = (vertices[i] - 0.5f) * width;
        vertices[i + 1] = (vertices[i + 1] - 0.5f) * height;
        vertices[i + 2] = (vertices[i + 2] - 0.5f) * length;
    }

    mesh.vertices = (float*)malloc(sizeof(vertices));
    mesh.texcoords = (float*)malloc(sizeof(texcoords));

    memcpy(mesh.vertices, vertices, sizeof(vertices));
    memcpy(mesh.texcoords, texcoords, sizeof(texcoords));

    // Yeah I ain't dealing with indices enjoy your 12 triangles
    mesh.vertexCount = sizeof(vertices) / sizeof(float) / 3;
    mesh.triangleCount = mesh.vertexCount / 3;

    UploadMesh(&mesh, false);
    return mesh;
}


Renderer::~Renderer() {
    UnloadShader(part_shader);
    UnloadShader(post_shader);
    UnloadShader(blur_shader);
    UnloadShader(grid_shader);

    UnloadRenderTexture(blur1_tex);
    UnloadRenderTexture(blur2_tex);
    UnloadRenderTexture(blur_tmp_tex);

    UnloadModel(grid_model);

    glDeleteBuffers(1, &ssbo_constants);
    glDeleteBuffers(1, &ubo_settings);
    glDeleteBuffers(1, &ssbo_display_mode);
    glDeleteTextures(BUFFER_COUNT, ao_tex);
    glDeleteTextures(BUFFER_COUNT, shadow_tex);
    delete[] ao_data;
    delete settings_writer;
}

void Renderer::_generate_render_textures() {
    UnloadRenderTexture(blur1_tex);
    UnloadRenderTexture(blur2_tex);
    UnloadRenderTexture(blur_tmp_tex);

    base_tex = MultiTexture(GetScreenWidth() / downscaleRatio, GetScreenHeight() / downscaleRatio);

    const unsigned int blur_width = GetScreenWidth() / blurDownscaleRatio;
    const unsigned int blur_height = GetScreenHeight() / blurDownscaleRatio;
    blur1_tex = util::load_render_texture_only_color(blur_width, blur_height, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    blur2_tex = util::load_render_texture_only_color(blur_width, blur_height, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    blur_tmp_tex = util::load_render_texture_only_color(blur_width, blur_height, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    // Prevent blur from wrapping around
    for (unsigned int texId : std::array<unsigned int, 5>({ 
        blur_tmp_tex.texture.id,
        blur1_tex.texture.id,
        blur2_tex.texture.id,
        base_tex.glowOnlyTexture,
        base_tex.blurOnlyTexture
    })) {
        rlTextureParameters(texId, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_MIRROR_REPEAT);
        rlTextureParameters(texId, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_MIRROR_REPEAT);
    }
}

void Renderer::init() {
#ifdef EMBED_SHADERS
    #include "../../resources/shaders/generated/fullscreen.vs.h"
    #include "../../resources/shaders/generated/part.fs.h"
    #include "../../resources/shaders/generated/post.fs.h"
    #include "../../resources/shaders/generated/blur.fs.h"
    #include "../../resources/shaders/generated/grid.fs.h"

    part_shader = LoadShaderFromMemory(fullscreen_vs_source, part_fs_source);
    post_shader = LoadShaderFromMemory(fullscreen_vs_source, post_fs_source);
    blur_shader = LoadShaderFromMemory(fullscreen_vs_source, blur_fs_source);
    grid_shader = LoadShaderFromMemory(nullptr, grid_fs_source);
#else
    part_shader = LoadShader("resources/shaders/fullscreen.vs", "resources/shaders/part.fs");
    post_shader = LoadShader("resources/shaders/fullscreen.vs", "resources/shaders/post.fs");
    blur_shader = LoadShader("resources/shaders/fullscreen.vs", "resources/shaders/blur.fs");
    grid_shader = LoadShader(nullptr, "resources/shaders/grid.fs");
#endif

    ao_data = new uint8_t[sim->graphics.ao_blocks.size()];
    _generate_render_textures();

    grid_max_dim = std::max({ XRES, YRES, ZRES });
    Mesh mesh = GenInvertedMeshCube((float)XRES, (float)YRES, (float)ZRES);
    grid_model = LoadModelFromMesh(mesh);
    grid_shader_size_loc = GetShaderLocation(grid_shader, "size");
    grid_shader_scale_loc = GetShaderLocation(grid_shader, "scale");
    grid_model.materials[0].shader = grid_shader;
    set_grid_size(0.0);

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
    colorBuf = util::PersistentBuffer<BUFFER_COUNT>(GL_SHADER_STORAGE_BUFFER, XRES * YRES * ZRES * sizeof(uint32_t), util::PBFlags::WRITE);
    flagBuf  = util::PersistentBuffer<BUFFER_COUNT>(GL_SHADER_STORAGE_BUFFER, XRES * YRES * ZRES * sizeof(uint8_t), util::PBFlags::WRITE);
    lodBuf   = util::PersistentBuffer<BUFFER_COUNT>(GL_SHADER_STORAGE_BUFFER, 
                sizeof(uint8_t) * OctreeBlockMetadata::layer_offsets[OCTREE_BLOCK_DEPTH - 1]
                * X_BLOCKS * Y_BLOCKS * Z_BLOCKS, util::PBFlags::WRITE);

    // Ambient occlusion texture, uses texture for free linear filtering
    glGenTextures(BUFFER_COUNT, ao_tex);
    for (std::size_t i = 0; i < BUFFER_COUNT; i++) {
        glBindTexture(GL_TEXTURE_3D, ao_tex[i]);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, AO_X_BLOCKS, AO_Y_BLOCKS, AO_Z_BLOCKS, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    }

    // Shadow texture
    glGenTextures(BUFFER_COUNT, shadow_tex);
    for (std::size_t i = 0; i < BUFFER_COUNT; i++) {
        glBindTexture(GL_TEXTURE_2D, shadow_tex[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SHADOW_MAP_X, SHADOW_MAP_Y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    }

    // SSBO display mode
    {
        ssbo_display_mode = rlLoadShaderBuffer(sizeof(SSBO_DisplayMode_t), NULL, RL_STATIC_READ);
        SSBO_DisplayMode_t data { .display_mode = static_cast<uint32_t>(sim->graphics.display_mode) };
        rlUpdateShaderBuffer(ssbo_display_mode, &data, sizeof(data), 0);
    }

    // SSBO constants
    // These are effectively uniform, however AMD cards default to std140
    // for `shared` layout, so array offsets are broken.
    // DO NOT USE ARRAYS IN UBOS unless you defined them as std140 and correctly
    // pad every array element to 16 bytes
    {
    #ifdef DEBUG
        if (cam->camera.fovy == 0.0)
            throw std::invalid_argument("Render camera fov should not be 0 (renderer should be initialized AFTER the camera)");
        if (HEAT_GRADIENT_STEPS != graphics::gradients::heat_gradient.size())
            throw std::invalid_argument("Heat gradient step count differs from expected constant");
    #endif

        ssbo_constants = rlLoadShaderBuffer(sizeof(SSBO_Constants_t), NULL, RL_STATIC_READ);
        SSBO_Constants_t data {
            .SIMRES{ (float)XRES, (float)YRES, (float)ZRES },
            .NUM_LEVELS = OCTREE_BLOCK_DEPTH,
            .FOV_DIV2 = cam->camera.fovy * DEG2RAD / 2.0f,
            .MOD_MASK = (1 << OCTREE_BLOCK_DEPTH) - 1,
            .AO_BLOCK_SIZE = AO_BLOCK_SIZE,
            .OCTTREE_BLOCK_DIMS{ X_BLOCKS, Y_BLOCKS, Z_BLOCKS },
            .AO_BLOCK_DIMS{ AO_X_BLOCKS, AO_Y_BLOCKS, AO_Z_BLOCKS }
        };
        memcpy(&data.LAYER_OFFSETS, &OctreeBlockMetadata::layer_offsets[0],
            OctreeBlockMetadata::layer_offsets.size() * sizeof(unsigned int));
        memcpy(&data.MORTON_X_SHIFTS, &Morton::X_SHIFTS[0], sizeof(Morton::X_SHIFTS));
        memcpy(&data.MORTON_Y_SHIFTS, &Morton::Y_SHIFTS[0], sizeof(Morton::Y_SHIFTS));
        memcpy(&data.MORTON_Z_SHIFTS, &Morton::Z_SHIFTS[0], sizeof(Morton::Z_SHIFTS));
        for (int i = 0; i < graphics::gradients::heat_gradient.size(); i++)
            data.HEAT_GRADIENT[i] = graphics::gradients::heat_gradient[i].as_ABGR();

        rlUpdateShaderBuffer(ssbo_constants, &data, sizeof(data), 0);
    }

    // UBO: settings
    {
        glGenBuffers(1, &ubo_settings);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_settings);
        settings_writer = new UBOWriter(part_shader.id, ubo_settings, "Settings");
        glBufferData(GL_UNIFORM_BUFFER, settings_writer->size(), NULL, GL_STATIC_DRAW);

        float BG_COLOR[] = { BACKGROUND_COLOR.r / 255.0f, BACKGROUND_COLOR.g / 255.0f, BACKGROUND_COLOR.b / 255.0f };
        float SH_COLOR[] = { SHADOW_COLOR.r / 255.0f, SHADOW_COLOR.g / 255.0f, SHADOW_COLOR.b / 255.0f };
        settings_writer->write_member("MAX_RAY_STEPS", 256 * 3);
        settings_writer->write_member("DEBUG_MODE", FragDebugMode::NODEBUG);
        settings_writer->write_member("AO_STRENGTH", 0.6f);
        settings_writer->write_member("BACKGROUND_COLOR", BG_COLOR);
        settings_writer->write_member("SHADOW_STRENGTH", 0.35f);
        settings_writer->write_member("SHADOW_COLOR", SH_COLOR);

        settings_writer->write_member("ENABLE_TRANSPARENCY", 1);
        settings_writer->write_member("ENABLE_REFLECTION", 1);
        settings_writer->write_member("ENABLE_REFRACTION", 1);
        settings_writer->write_member("ENABLE_BLUR", 1);
        settings_writer->write_member("ENABLE_GLOW", 1);
        settings_writer->write_member("ENABLE_AO", 1);
        settings_writer->write_member("ENABLE_SHADOWS", 1);
        settings_writer->write_member("HEAT_VIEW_MIN", MIN_TEMP);
        settings_writer->write_member("HEAT_VIEW_MAX", 5000.0f);

        settings_writer->upload();
    }

    update_settings(settings::data::ref()->graphics);
}

void Renderer::set_grid_size(float size) {
    grid_scale = size;
    util::set_shader_value(grid_shader, grid_shader_size_loc, (float)grid_max_dim);
    util::set_shader_value(grid_shader, grid_shader_scale_loc, size);
}

void Renderer::update_settings(settings::Graphics * settings) {
    show_octree = settings->showOctree;
    do_blur = settings->enableBlur;
    do_glow = settings->enableGlow;
    do_ao = settings->enableAO;
    do_shadows = settings->enableShadows;
    downscaleRatio = blurDownscaleRatio = settings->renderDownscale;

    if (settings->fullScreen != IsWindowFullscreen()) {
        if (!IsWindowFullscreen()) { // To fullscreen
            preFullscreenWindowRes = Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() };
            const int monitor = GetCurrentMonitor();
            SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
            ToggleFullscreen();
        } else {
            ToggleFullscreen();
            SetWindowSize(preFullscreenWindowRes.x, preFullscreenWindowRes.y);
        }
    }

    settings_writer->write_member("DEBUG_MODE", (FragDebugMode)settings->renderMode);
    settings_writer->write_member("AO_STRENGTH", settings->aoStrength);
    settings_writer->write_member("SHADOW_STRENGTH", settings->shadowStrength);

    settings_writer->write_member("ENABLE_TRANSPARENCY", settings->enableTransparency ? 1 : 0);
    settings_writer->write_member("ENABLE_REFLECTION", settings->enableReflection ? 1 : 0);
    settings_writer->write_member("ENABLE_REFRACTION", settings->enableRefraction ? 1 : 0);
    settings_writer->write_member("ENABLE_BLUR", settings->enableBlur ? 1 : 0);
    settings_writer->write_member("ENABLE_GLOW", settings->enableGlow ? 1 : 0);
    settings_writer->write_member("ENABLE_AO", settings->enableAO ? 1 : 0);
    settings_writer->write_member("ENABLE_SHADOWS", settings->enableShadows ? 1 : 0);
    settings_writer->write_member("HEAT_VIEW_MIN", settings->heatViewMin);
    settings_writer->write_member("HEAT_VIEW_MAX", settings->heatViewMax);
    settings_writer->upload();

    _generate_render_textures();
}

void Renderer::update_colors_and_lod() {
    const unsigned int ssbo_idx = (frame_count + 1) % BUFFER_COUNT;
    const unsigned int ssbo_bit = 1 << ssbo_idx;

    colorBuf.wait();
    flagBuf.wait();

    for (std::size_t i = 0; i < COLOR_DATA_CHUNK_COUNT; i++) {
        if (sim->graphics.color_data_modified[i] & ssbo_bit) {
            std::copy(&sim->graphics.color_data[i * COLOR_DATA_CHUNK_SIZE],
                &sim->graphics.color_data[(i + 1) * COLOR_DATA_CHUNK_SIZE],
                &colorBuf.get<uint32_t>(0)[i * COLOR_DATA_CHUNK_SIZE]);

            std::copy(&sim->graphics.color_flags[i * COLOR_DATA_CHUNK_SIZE],
                &sim->graphics.color_flags[(i + 1) * COLOR_DATA_CHUNK_SIZE],
                &flagBuf.get<uint8_t>(0)[i * COLOR_DATA_CHUNK_SIZE]);

            sim->graphics.color_data_modified[i] &= ~ssbo_bit;
        }
    }

    colorBuf.lock();
    flagBuf.lock();

    lodBuf.wait();
    for (std::size_t i = 0; i < sim->graphics.octree_blocks.size(); i++) {
        if (sim->graphics.octree_blocks[i].modified & ssbo_bit) {
            // We do not upload the whole octree here, we upload all layers except
            // the last layer, since the last layer only stores info about the 1x1x1 voxel
            // data which we already have in the form of color_data
            std::copy(
                &sim->graphics.octree_blocks[i].data[0],
                &sim->graphics.octree_blocks[i].data[OctreeBlockMetadata::layer_offsets[OCTREE_BLOCK_DEPTH - 1]],
                &lodBuf.get<uint8_t>(0)[i * OctreeBlockMetadata::layer_offsets[OCTREE_BLOCK_DEPTH - 1]]
            );
            sim->graphics.octree_blocks[i].modified &= ~ssbo_bit;
        }
    }
    lodBuf.lock();

    if (do_ao) {
        glBindTexture(GL_TEXTURE_3D, ao_tex[ssbo_idx]);
        constexpr unsigned int AO_VOLUME = AO_BLOCK_SIZE * AO_BLOCK_SIZE * AO_BLOCK_SIZE;
        #pragma omp simd
        for (std::size_t i = 0; i < sim->graphics.ao_blocks.size(); i++)
            ao_data[i] = 255 * sim->graphics.ao_blocks[i] / AO_VOLUME;
        glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, AO_X_BLOCKS, AO_Y_BLOCKS, AO_Z_BLOCKS, GL_RED, GL_UNSIGNED_BYTE, ao_data);
    }

    if (do_shadows) {
        glBindTexture(GL_TEXTURE_2D, shadow_tex[ssbo_idx]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SHADOW_MAP_X, SHADOW_MAP_Y, GL_RED, GL_UNSIGNED_BYTE, sim->graphics.shadow_map);
    }

    if (sim->graphics.display_mode != cached_display_mode) {
        cached_display_mode = sim->graphics.display_mode;
        ssbo_display_mode = rlLoadShaderBuffer(sizeof(SSBO_DisplayMode_t), NULL, RL_STATIC_READ);
        SSBO_DisplayMode_t data { .display_mode = static_cast<uint32_t>(sim->graphics.display_mode) };
        rlUpdateShaderBuffer(ssbo_display_mode, &data, sizeof(data), 0);
    }
}

void Renderer::draw_octree_debug() {
    BeginMode3D(cam->camera);
    for (std::size_t i = 0; i < sim->graphics.octree_blocks.size(); i++) {
        if (!sim->graphics.octree_blocks[i].data[0]) continue;
        int blockX = i % X_BLOCKS;
        int blockY = (i / X_BLOCKS) % Y_BLOCKS;
        int blockZ = (i / X_BLOCKS / Y_BLOCKS);

        for (std::size_t layer = OCTREE_BLOCK_DEPTH - 3; layer <= OCTREE_BLOCK_DEPTH; layer++) {
        for (std::size_t dz = 0; dz < (OCTREE_BLOCK_DIM >> layer); dz++) {
        for (std::size_t dy = 0; dy < (OCTREE_BLOCK_DIM >> layer); dy++) {
        for (std::size_t dx = 0; dx < (OCTREE_BLOCK_DIM >> layer); dx++) {
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
    EndMode3D();
}

void Renderer::draw() {
    if (IsWindowResized())
        _generate_render_textures();

    bool displayModeChanged = sim->graphics.display_mode != cached_display_mode;
    update_colors_and_lod();
    if (show_octree)
        draw_octree_debug();

#pragma region uniforms
    const Vector2 resolution{ (float)GetScreenWidth(), (float)GetScreenHeight() };
    const Vector2 virtual_resolution{ (float)GetScreenWidth() / downscaleRatio, (float)GetScreenHeight() / downscaleRatio };
    const Vector2 blur_resolution{ (float)GetScreenWidth() / blurDownscaleRatio, (float)GetScreenHeight() / blurDownscaleRatio };

    // Inverse camera rotation matrix
    auto transform_mat = MatrixLookAt(cam->camera.position, cam->camera.target, cam->camera.up);
    util::reduce_to_rotation(transform_mat);
    const auto transform_matT = MatrixTranspose(transform_mat);

    const float aspect_ratio = resolution.x / resolution.y;
    const auto look_ray = Vector3Normalize(cam->camera.target - cam->camera.position);

    // Actual 3D vectors in world space that correspond to screen RIGHT (uv1) and screen UP (uv2)
    const Vector3 uv1 = Vector3Transform(Vector3{1.0, 0.0, 0.0} * aspect_ratio, transform_matT);
    const Vector3 uv2 = Vector3Transform(Vector3{0.0, 1.0, 0.0}, transform_matT);

#pragma endregion uniforms

    // First actual pass
    // --------------------------------------
    const unsigned int ssbo_idx = frame_count % BUFFER_COUNT;
    rlBindShaderBuffer(colorBuf.getId(0), 0);
    rlBindShaderBuffer(flagBuf.getId(0), 1);
    rlBindShaderBuffer(lodBuf.getId(0), 2);

    colorBuf.advance_cycle();
    flagBuf.advance_cycle();
    lodBuf.advance_cycle();

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_3D, ao_tex[ssbo_idx]);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadow_tex[ssbo_idx]);

    rlBindShaderBuffer(ssbo_constants, 5);
    glBindBufferBase(GL_UNIFORM_BUFFER, 6, ubo_settings);
    rlBindShaderBuffer(ssbo_display_mode, 7);
    rlBindShaderBuffer(sim->heat.get_heat_in_ssbo(), 8);

    // First render everything to FBO
    // which contains multiple textures for glow, blur, base, depth, etc...
    bool isPersistentDisplay = sim->graphics.display_mode == DisplayMode::DISPLAY_MODE_PERSISTENT;

    glClearColor(0, 0, 0, 0);
    rlEnableFramebuffer(base_tex.frameBuffer);
    if (isPersistentDisplay && !displayModeChanged) {
        glClear(GL_DEPTH_BUFFER_BIT);  
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 3});
    } else
        rlClearScreenBuffers();

    BeginMode3D(cam->camera);
    BeginShaderMode(part_shader);

        util::set_shader_value(part_shader, part_shader_res_loc, virtual_resolution);
        util::set_shader_value(part_shader, part_shader_camera_pos_loc, cam->camera.position);
        util::set_shader_value(part_shader, part_shader_camera_dir_loc, look_ray);
        util::set_shader_value(part_shader, part_shader_uv1_loc, uv1);
        util::set_shader_value(part_shader, part_shader_uv2_loc, uv2);
        util::draw_dummy_triangle();

    EndShaderMode();
    EndMode3D();
    rlDisableFramebuffer();

    auto glow_tex_id = base_tex.glowOnlyTexture;
    auto blur_tex_id = base_tex.blurOnlyTexture;
    bool isFancyDisplay = sim->graphics.display_mode == DisplayMode::DISPLAY_MODE_FANCY; // Check also in part.fs

    if (do_glow && isFancyDisplay) {
        _blur_render_texture(base_tex.glowOnlyTexture, blur_resolution, blur1_tex);
        glow_tex_id = blur1_tex.texture.id;
    }
    if (do_blur && isFancyDisplay) {
        _blur_render_texture(base_tex.blurOnlyTexture, blur_resolution, blur2_tex);
        blur_tex_id = blur2_tex.texture.id;
    }

    // Render the above textures with a post-processing shader for compositing
    BeginMode3D(cam->camera);
        DrawModel(grid_model, Vector3{XRES / 2.0f, YRES / 2.0f, ZRES / 2.0f}, 1.0f, WHITE);

        BeginShaderMode(post_shader);

            rlEnableShader(post_shader.id);
            rlSetUniformSampler(post_shader_base_texture_loc, base_tex.colorTexture);
            rlSetUniformSampler(post_shader_glow_texture_loc, glow_tex_id);
            rlSetUniformSampler(post_shader_blur_texture_loc, blur_tex_id);
            rlSetUniformSampler(post_shader_depth_texture_loc, base_tex.depthTexture);
            util::set_shader_value(post_shader, post_shader_res_loc, resolution);

            util::draw_dummy_triangle();
            glBindTexture(GL_TEXTURE_2D, 0);
            rlDisableShader();

        EndShaderMode();
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
    util::set_shader_value(blur_shader, blur_shader_res_loc, resolution);

    for (int pass = 0; pass < 4; pass++) {
        // First pass: use textureInId, rest use output of previous pass
        if (pass > 0) textureInId = blur_tex.texture.id;

        // Split into 2 subpasses: horizontal and vertical
        for (int i = 0; i < 2; i++) {
            BeginTextureMode(i == 0 ? blur_tmp_tex : blur_tex);
                ClearBackground(BLANK);
                util::set_shader_value(blur_shader, blur_shader_dir_loc, Vector2{ float(i), float(1 - i) });
                rlSetUniformSampler(blur_shader_base_texture_loc, i == 0 ? textureInId : blur_tmp_tex.texture.id);
                util::draw_dummy_triangle();
            EndTextureMode();
        }
    }
    EndShaderMode();
}
