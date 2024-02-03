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
    glDeleteBuffers(1, &ssbo_colors);
    glDeleteBuffers(1, &ssbo_lod);
    glDeleteBuffers(1, &ubo_constants);
}

void Renderer::init() {
    part_shader = LoadShader("resources/shaders/part.vs", "resources/shaders/part.fs");

    // Uniform values that may change per frame
    part_shader_res_loc = GetShaderLocation(part_shader, "resolution");
    part_shader_camera_pos_loc = GetShaderLocation(part_shader, "cameraPos");
    part_shader_camera_dir_loc = GetShaderLocation(part_shader, "cameraDir");
    part_shader_uv1_loc = GetShaderLocation(part_shader, "uv1");
    part_shader_uv2_loc = GetShaderLocation(part_shader, "uv2");

    // SSBOs for color & octree LOD data & ambient occlusion
    ssbo_colors = rlLoadShaderBuffer(XRES * YRES * ZRES * sizeof(uint32_t), NULL, RL_DYNAMIC_COPY);
    ssbo_lod = rlLoadShaderBuffer(sizeof(uint8_t) *  OctreeBlockMetadata::size * X_BLOCKS * Y_BLOCKS * Z_BLOCKS,
        NULL, RL_DYNAMIC_COPY);
    ssbo_ao = rlLoadShaderBuffer(AO_X_BLOCKS * AO_Y_BLOCKS * AO_Z_BLOCKS * sizeof(unsigned int), NULL, RL_DYNAMIC_COPY);

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

        settings_writer.write_member("MAX_RAY_STEPS", 256 * 2);
        settings_writer.write_member("DEBUG_MODE", FragDebugMode::DEBUG_AO);
        settings_writer.write_member("AO_STRENGTH", 0.6f);
        settings_writer.upload();
    }
}

void Renderer::update_colors_and_lod() {
    for (std::size_t i = 0; i < COLOR_DATA_CHUNK_COUNT; i++) {
        if (sim->color_data_modified[i]) {
            rlUpdateShaderBuffer(ssbo_colors,
                &sim->color_data[i * COLOR_DATA_CHUNK_SIZE],
                COLOR_DATA_CHUNK_SIZE * sizeof(uint32_t),
                i * COLOR_DATA_CHUNK_SIZE * sizeof(uint32_t));
            sim->color_data_modified[i] = false;
        }
    }

    for (std::size_t i = 0; i < sim->octree_blocks.size(); i++) {
        if (sim->octree_blocks[i].modified) {
            // We do not upload the whole octree here, we upload all layers except
            // the last layer, since the last layer only stores info about the 1x1x1 voxel
            // data which we already have in the form of color_data
            rlUpdateShaderBuffer(ssbo_lod, sim->octree_blocks[i].data,
                sizeof(uint8_t) * OctreeBlockMetadata::layer_offsets[OCTREE_BLOCK_DEPTH - 1],
                i * sizeof(uint8_t) * OctreeBlockMetadata::layer_offsets[OCTREE_BLOCK_DEPTH - 1]);
            sim->octree_blocks[i].modified = false;
        }
    }

    // TODO: also diff ambient occlusion blocks
    rlUpdateShaderBuffer(ssbo_ao, sim->ao_blocks.data(), sizeof(unsigned int) * AO_X_BLOCKS * AO_Y_BLOCKS * AO_Z_BLOCKS, 0);
}

void Renderer::draw_octree_debug() {
    for (std::size_t i = 0; i < sim->octree_blocks.size(); i++) {
        if (!sim->octree_blocks[i].data[0]) continue;
        int blockX = i % X_BLOCKS;
        int blockY = (i / X_BLOCKS) % Y_BLOCKS;
        int blockZ = (i / X_BLOCKS / Y_BLOCKS);

        for (int layer = OCTREE_BLOCK_DEPTH - 3; layer <= OCTREE_BLOCK_DEPTH; layer++) {
        for (int dz = 0; dz < (OCTREE_BLOCK_DIM >> layer); dz++) {
        for (int dy = 0; dy < (OCTREE_BLOCK_DIM >> layer); dy++) {
        for (int dx = 0; dx < (OCTREE_BLOCK_DIM >> layer); dx++) {
            unsigned int morton = util::morton_decode8(dx, dy, dz);

            if (sim->octree_blocks[i].data[morton + OctreeBlockMetadata::layer_offsets[OCTREE_BLOCK_DEPTH - layer]] != 0) {
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

    rlBindShaderBuffer(ssbo_colors, 0);
    rlBindShaderBuffer(ssbo_lod, 1);
    rlBindShaderBuffer(ssbo_ao, 2);
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, ubo_constants);
    glBindBufferBase(GL_UNIFORM_BUFFER, 4, ubo_settings);

    BeginShaderMode(part_shader);

    // Inverse camera rotation matrix
    auto transform_mat = MatrixLookAt(cam->camera.position, cam->camera.target, cam->camera.up);
    util::reduce_to_rotation(transform_mat);
    const auto transform_matT = MatrixTranspose(transform_mat);

    const Vector2 resolution{ (float)GetScreenWidth(), (float)GetScreenHeight() };
    const float aspect_ratio = resolution.x / resolution.y;
    const auto look_ray = Vector3Normalize(cam->camera.target - cam->camera.position);

    // Actual 3D vectors in world space that correspond to screen RIGHT (uv1) and screen UP (uv2)
    const Vector3 uv1 = Vector3Transform(Vector3{1.0, 0.0, 0.0} * aspect_ratio, transform_matT);
    const Vector3 uv2 = Vector3Transform(Vector3{0.0, 1.0, 0.0}, transform_matT);

    util::set_shader_value(part_shader, part_shader_res_loc, resolution);
    util::set_shader_value(part_shader, part_shader_camera_pos_loc, cam->camera.position);
    util::set_shader_value(part_shader, part_shader_camera_dir_loc, look_ray);
    util::set_shader_value(part_shader, part_shader_uv1_loc, uv1);
    util::set_shader_value(part_shader, part_shader_uv2_loc, uv2);

    // Draw fullscreen triangle, the overdraw has 0 effect on performance
    Vector3 cent = cam->camera.position + look_ray; // Center pos of triangle
    DrawTriangle3D(
        cent - 2.0f * uv1 + uv2,
        cent + uv1 - 2.0 * uv2,
        cent + uv1 + uv2,
        WHITE
    );
    
    EndShaderMode();
}
