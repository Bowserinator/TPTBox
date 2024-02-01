#include "Renderer.h"
#include "../simulation/Simulation.h"
#include "../simulation/ElementClasses.h"
#include "camera/camera.h"
#include "constants.h"
#include "../util/math.h"
#include "../util/graphics.h"

#include "rlgl.h"
#include "stdint.h"
#include <cstring>

void Renderer::init() {
    part_shader = LoadShader("resources/shaders/part.vs", "resources/shaders/part.fs");

    // for (int z = 1; z < ZRES - 1; z++) {
    //     for (int y = 1; y < YRES - 1; y++) {
    //         for (int x = 1; x < XRES - 1 ; x++) {
    //             int i = x + y * XRES + z * YRES * XRES;

    //             if (y > YRES - 3 || z < 2)
    //                 color_data[i] = 0x3FFFFFFF;
    //             if (x < 4 && x > 2)
    //                 color_data[i] = 0x3F0000FF;
    //             if (y < 3)
    //                 color_data[i] = 0x3F00FF00;
    //             if (x > XRES - 3)
    //                 color_data[i] = 0xFF00FF00;

    //             if (x == XRES / 2 && y == YRES / 2 && z == ZRES / 2)
    //                 color_data[i] = 0xFF0000FF;
    //             if (x == XRES / 2 - 1 && y == YRES / 2 && z == ZRES / 2)
    //                 color_data[i] = 0xFFFF00FF;

    //             float s = 1/10.0;
    //             float x2 = (x - XRES / 2.0) * s;
    //             float y2 = (y - YRES / 2.0) * s;
    //             float z2 = (z - ZRES / 2.0) * s;
    //             float r = 0.5 * (x2*x2*x2*x2 + y2*y2*y2*y2 + z2*z2*z2*z2) - 8 * (x2*x2+y2*y2+z2*z2) + 60;
    //             // r = std::pow(4 - std::sqrt(x2*x2 + z2*z2), 2) + y2*y2 - 4;

    //             if (fabs(x - (float)XRES / 2) < 20 && fabs(y - (float)YRES / 2) < 20 && fabs(z - ZRES / 2.0) < 20) {
    //                 color_data[i] = 0xDDFF0000; // ABGR
    //             }

    //             // if (r < 0) {
    //             //     color_data[i] = 0xFF000000; // ABGR
    //             //     unsigned char r2 = x % 256;
    //             //     unsigned char g = y % 256;
    //             //     unsigned char b = z % 256;
    //             //     color_data[i] += r2 + 256 * g + 256 * 256 * b;
    //             // }
    //         //     // if (std::hypot(x - XRES * 0.25, y - YRES / 2.0, z - ZRES / 2.0) < 40) {
    //             // if (fabs(x - (float)XRES / 4) < 2 && fabs(y - (float)YRES / 2) < 20 && fabs(z - ZRES / 2.0) < 20) {
    //             //     color_data[i] = 0x22FF0000; // ABGR
    //             // }
    //             // if (fabs(x - (float)XRES / 4 - 6) < 2 && fabs(y - (float)YRES / 2) < 10 && fabs(z - ZRES / 2.0 ) < 10) {
    //             //     color_data[i] = 0x22FF0000; // ABGR
    //             // }
    //             // if (fabs(x - 3 * (float)XRES / 4) < 20 && fabs(y - (float)YRES / 2) < 20 && fabs(z - ZRES / 2.0) < 20) {
    //             //     color_data[i] = 0xFFFF0000; // ABGR
    //             // }
    //             // if (fabs(x - 3 * (float)XRES / 4) < 19 && fabs(y - (float)YRES / 2) < 19 && fabs(z - ZRES / 2.0) < 19) {
    //             //     color_data[i] = 0xFFFFFF00; // ABGR
    //             // }
    //             // if (fabs(x - 3 * (float)XRES / 4) < 18 && fabs(y - (float)YRES / 2) < 18 && fabs(z - ZRES / 2.0) < 18) {
    //             //     color_data[i] = 0xFFFFFFFF; // ABGR
    //             // }


    //             // if ((x/2 + y/2 + z/2) % 2 == 0)
    //             //     continue;
    //             // if (y % 4 != 0)
    //             //     continue;
    //             // if (util::hypot(x - XRES / 2, y - YRES / 2, z - ZRES / 2) > 60.0f)
    //             //     continue;
    //         }
    //     }
    // }

 
    part_shader_res_loc = GetShaderLocation(part_shader, "resolution");
    part_shader_camera_pos_loc = GetShaderLocation(part_shader, "cameraPos");
    part_shader_camera_dir_loc = GetShaderLocation(part_shader, "cameraDir");
    part_shader_sim_res_loc = GetShaderLocation(part_shader, "simRes");
    part_shader_uv1_loc = GetShaderLocation(part_shader, "uv1");
    part_shader_uv2_loc = GetShaderLocation(part_shader, "uv2");

    ssbo_colors = rlLoadShaderBuffer(XRES * YRES * ZRES * sizeof(uint32_t), NULL, RL_DYNAMIC_COPY);
    ssbo_lod = rlLoadShaderBuffer(sizeof(uint8_t) *  OctreeBlockMetadata::size * X_BLOCKS * Y_BLOCKS * Z_BLOCKS,
        NULL, RL_DYNAMIC_COPY);
}


void Renderer::update_texture(Simulation * sim, RenderCamera * cam) {
    // Update SSBO values here

    for (std::size_t i = 0; i < COLOR_DATA_CHUNK_COUNT; i++) {
        if (sim->color_data_modified[i]) {
            rlUpdateShaderBuffer(ssbo_colors,
                &sim->color_data[i * COLOR_DATA_CHUNK_SIZE],
                COLOR_DATA_CHUNK_SIZE * sizeof(uint32_t),
                i * COLOR_DATA_CHUNK_SIZE * sizeof(uint32_t));
            sim->color_data_modified[i] = false;
        }
    }

    // uint8_t * test = new uint8_t[OctreeBlockMetadata::size * 64];

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
       //  memcpy(test + i * OctreeBlockMetadata::size, sim->octree_blocks[i].data, 37499);
    }


    
    // for (int z = 0; z < 16; z++) {
    // for (int y = 0; y < 16; y++) {
    // for (int x = 0; x < 16; x++) {

    //         int level = 4;
    //         int x2 = x << (level); // say level = 6
    //         int y2 = y << (level);
    //         int z2 = z << (level);
    //         // TODO: compile these constants into shaders
    //         int chunk_offset = (x2 / 64) + (y2 / 64) * 4 + (z2 / 64) * 4 * 4;

    //         // std::cout << chunk_offset << " HUNK OFFSET\n";

    //         unsigned int morton = morton_decode2(x2 % 64, y2 % 64, z2 % 64) >> (3 * (level ));
    //         unsigned int bit_idx = morton & 7;
    //         unsigned int offset = 1;
    //         morton >>= 3;

    //         if (test[chunk_offset * OctreeBlockMetadata::size + offset + morton] & (1 << bit_idx) != 0) {
    //             DrawCubeWires(Vector3{x2 + 8, y2 + 8, z2 + 8}, 16, 16, 16, Color{255, 0, 0, 250});
    //         }

    //         // if (test[chunk_offset * OctreeBlockMetadata::size] != 0) {
    //         //     DrawCubeWires(Vector3{x2 + 32, y2 + 32, z2 + 32}, 64, 64, 64, Color{255, 0, 0, 250});
    //         // }
    //     }
    // }
    // }
    // delete[] test;
}



void Renderer::draw(Simulation * sim, RenderCamera * cam) {
    update_texture(sim, cam);

    // Bind SSBOs to render shader
    rlBindShaderBuffer(ssbo_colors, 0);
    rlBindShaderBuffer(ssbo_lod, 1);
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
    util::set_shader_value(part_shader, part_shader_sim_res_loc, Vector3{ XRES, YRES, ZRES });
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
