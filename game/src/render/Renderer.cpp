#include "Renderer.h"
#include "../simulation/Simulation.h"
#include "camera/camera.h"
#include "constants.h"
#include "../util/util.h"
#include "../util/graphics.h"

#include "rlgl.h"
#include "stdint.h"
#include <cstring>

void Renderer::init() {
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    char * data_arr = new char[ZRES * YRES * XRES];
    // memset(data_arr, 0xFF, sizeof(char) * XRES * YRES * ZRES / 2);
    for (int z = 0; z < ZRES; z++) {
        for (int y = 0; y < YRES; y++) {
            for (int x = 0; x < XRES ; x++) {
                int i = x + y * XRES + z * YRES * XRES;

                if ((x/2 + y/2 + z/2) % 2 == 0)
                    continue;
                // if (y % 4 != 0)
                //    continue;
                // if (util::hypot(x - XRES / 2, y - YRES / 2, z - ZRES / 2) > 30.0f)
                //     continue;
                data_arr[i] = 255;
            }
        }
    }

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, XRES, YRES, ZRES, 0, GL_RED, GL_UNSIGNED_BYTE, data_arr);
    glGenerateMipmap(GL_TEXTURE_3D);

    part_shader = LoadShader("resources/shaders/part.vs", "resources/shaders/part.fs");
    part_shader_color_loc = GetShaderLocation(part_shader, "colors");
    part_shader_res_loc = GetShaderLocation(part_shader, "resolution");
    part_shader_camera_pos_loc  = GetShaderLocation(part_shader, "cameraPos");
    part_shader_camera_dir_loc = GetShaderLocation(part_shader, "cameraDir");
    part_shader_sim_res_loc = GetShaderLocation(part_shader, "simRes");
    part_shader_uv1_loc =  GetShaderLocation(part_shader, "uv1");
    part_shader_uv2_loc =  GetShaderLocation(part_shader, "uv2");
}

void Renderer::draw(Simulation * sim, RenderCamera * cam) {
    BeginShaderMode(part_shader);

    auto transform_mat = MatrixLookAt(cam->camera.position, cam->camera.target, cam->camera.up);
    util::reduce_to_rotation(transform_mat);
    const auto transform_matT = MatrixTranspose(transform_mat);

    util::set_shader_value(part_shader, part_shader_res_loc, Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() });
    util::set_shader_value(part_shader, part_shader_camera_pos_loc, cam->camera.position);
    util::set_shader_value(part_shader, part_shader_sim_res_loc, Vector3{ XRES, YRES, ZRES });
    util::set_shader_value(part_shader, part_shader_camera_dir_loc,
        Vector3Normalize(cam->camera.target - cam->camera.position));
    util::set_shader_value(part_shader, part_shader_uv1_loc, Vector3Transform(Vector3{1.0, 0.0, 0.0}, transform_matT));
    util::set_shader_value(part_shader, part_shader_uv2_loc, Vector3Transform(Vector3{0.0, 1.0, 0.0}, transform_matT));

    // Draw cube plane
    rlBegin(RL_QUADS); // Front
        rlVertex3f(0.0f, 0.0f, 0.0f);
        rlVertex3f(0.0f, YRES, 0.0f);
        rlVertex3f(XRES, YRES, 0.0f);
        rlVertex3f(XRES, 0.0f, 0.0f);

        rlVertex3f(XRES, 0.0f, 0.0f);
        rlVertex3f(XRES, YRES, 0.0f);
        rlVertex3f(0.0f, YRES, 0.0f);
        rlVertex3f(0.0f, 0.0f, 0.0f);
    rlEnd();

    rlBegin(RL_QUADS); // Back
        rlVertex3f(XRES, 0.0f, ZRES);
        rlVertex3f(XRES, YRES, ZRES);
        rlVertex3f(0.0f, YRES, ZRES);
        rlVertex3f(0.0f, 0.0f, ZRES);

        rlVertex3f(0.0f, 0.0f, ZRES);
        rlVertex3f(0.0f, YRES, ZRES);
        rlVertex3f(XRES, YRES, ZRES);
        rlVertex3f(XRES, 0.0f, ZRES);
    rlEnd();

    rlBegin(RL_QUADS); // Bottom
        rlVertex3f(0.0f, 0.0f, 0.0f);
        rlVertex3f(0.0f, 0.0f, ZRES);
        rlVertex3f(XRES, 0.0f, ZRES);
        rlVertex3f(XRES, 0.0f, 0.0f);

        rlVertex3f(XRES, 0.0, 0.0f);
        rlVertex3f(XRES, 0.0, ZRES);
        rlVertex3f(0.0f, 0.0, ZRES);
        rlVertex3f(0.0f, 0.0, 0.0f);
    rlEnd();

    rlBegin(RL_QUADS); // Top
        rlVertex3f(0.0f, YRES, 0.0f);
        rlVertex3f(0.0f, YRES, ZRES);
        rlVertex3f(XRES, YRES, ZRES);
        rlVertex3f(XRES, YRES, 0.0f);

        rlVertex3f(XRES, YRES, 0.0f);
        rlVertex3f(XRES, YRES, ZRES);
        rlVertex3f(0.0f, YRES, ZRES);
        rlVertex3f(0.0f, YRES, 0.0f);
    rlEnd();

    rlBegin(RL_QUADS); // Left
        rlVertex3f(0.0f, 0.0f, ZRES);
        rlVertex3f(0.0f, YRES, ZRES);
        rlVertex3f(0.0f, YRES, 0.0f);
        rlVertex3f(0.0f, 0.0f, 0.0f);

        rlVertex3f(0.0f, 0.0f, 0.0f);
        rlVertex3f(0.0f, YRES, 0.0f);
        rlVertex3f(0.0f, YRES, ZRES);
        rlVertex3f(0.0f, 0.0f, ZRES);
    rlEnd();

    rlBegin(RL_QUADS); // Right
        rlVertex3f(XRES, 0.0f, ZRES);
        rlVertex3f(XRES, YRES, ZRES);
        rlVertex3f(XRES, YRES, 0.0f);
        rlVertex3f(XRES, 0.0f, 0.0f);

        rlVertex3f(XRES, 0.0f, 0.0f);
        rlVertex3f(XRES, 0.0f, ZRES);
        rlVertex3f(XRES, YRES, ZRES);
        rlVertex3f(XRES, YRES, 0.0f);
    rlEnd();
    EndShaderMode();
}
