#include "Renderer.h"
#include "../simulation/Simulation.h"
#include "camera/camera.h"
#include "constants.h"
#include "../util/util.h"
#include "../util/graphics.h"

#include "rlgl.h"
#include "stdint.h"
#include <cstring>

constexpr int maxlod = 6;

void Renderer::init() {
    part_shader = LoadShader("resources/shaders/part.vs", "resources/shaders/part.fs");

    using T = unsigned int;
    T * data_arr = new T[ ZRES * YRES * XRES];
    memset(data_arr, 0x0, sizeof(T) * XRES * YRES * ZRES);

    uint8_t ** data_lod = new uint8_t*[maxlod];
    for (int i = 1; i <= maxlod; i++) {
        int A = 1 << i;
        int s = (ZRES / A + 1) * (YRES / A + 1) * (XRES / A + 1);
        uint8_t * lod4_arr = new uint8_t[s];
        memset(lod4_arr, 0x00, sizeof(uint8_t) * s);
        data_lod[i - 1] = lod4_arr;
    }

    // int s = (ZRES / A + 1) * (YRES / A + 1) * (XRES / A + 1);
    // uint8_t * lod4_arr = new uint8_t[s];
    // memset(lod4_arr, 0x00, sizeof(uint8_t) * s);

    // s = (ZRES / B + 1) * (YRES / B + 1) * (XRES / B + 1);
    // uint8_t * lod16_arr = new uint8_t[s];
    // memset(lod16_arr, 0x00, sizeof(uint8_t) * s);


    for (int z = 1; z < ZRES - 1; z++) {
        for (int y = 1; y < YRES - 1; y++) {
            for (int x = 1; x < XRES - 1 ; x++) {
                int i = x + y * XRES + z * YRES * XRES;

                if (y > YRES - 3 || z < 2)
                    data_arr[i] = 0x0FFFFFFF;
                if (x < 4 && x > 2)
                    data_arr[i] = 0x0F0000FF;
                if (y < 3)
                    data_arr[i] = 0x0F00FF00;
                if (x > XRES - 3)
                    data_arr[i] = 0xFF00FF00;

                if (x == XRES / 2 && y == YRES / 2 && z == ZRES / 2)
                    data_arr[i] = 0xFF0000FF;
                if (x == XRES / 2 - 1 && y == YRES / 2 && z == ZRES / 2)
                    data_arr[i] = 0xFFFF00FF;

                float s = 1/10.0;
                float x2 = (x - XRES / 2.0) * s;
                float y2 = (y - YRES / 2.0) * s;
                float z2 = (z - ZRES / 2.0) * s;
                float r = 0.5 * (x2*x2*x2*x2 + y2*y2*y2*y2 + z2*z2*z2*z2) - 8 * (x2*x2+y2*y2+z2*z2) + 60;
                // r = std::pow(4 - std::sqrt(x2*x2 + z2*z2), 2) + y2*y2 - 4;

                if (r < 0) {
                    data_arr[i] = 0xFF000000; // ABGR
                    unsigned char r2 = x % 256;
                    unsigned char g = y % 256;
                    unsigned char b = z % 256;
                    data_arr[i] += r2 + 256 * g + 256 * 256 * b;
                }
            //     // if (std::hypot(x - XRES * 0.25, y - YRES / 2.0, z - ZRES / 2.0) < 40) {
                // if (fabs(x - (float)XRES / 4) < 2 && fabs(y - (float)YRES / 2) < 20 && fabs(z - ZRES / 2.0) < 20) {
                //     data_arr[i] = 0x22FF0000; // ABGR
                // }
                // if (fabs(x - (float)XRES / 4 - 6) < 2 && fabs(y - (float)YRES / 2) < 10 && fabs(z - ZRES / 2.0 ) < 10) {
                //     data_arr[i] = 0x22FF0000; // ABGR
                // }
                // if (fabs(x - 3 * (float)XRES / 4) < 20 && fabs(y - (float)YRES / 2) < 20 && fabs(z - ZRES / 2.0) < 20) {
                //     data_arr[i] = 0xFFFF0000; // ABGR
                // }
                // if (fabs(x - 3 * (float)XRES / 4) < 19 && fabs(y - (float)YRES / 2) < 19 && fabs(z - ZRES / 2.0) < 19) {
                //     data_arr[i] = 0xFFFFFF00; // ABGR
                // }
                // if (fabs(x - 3 * (float)XRES / 4) < 18 && fabs(y - (float)YRES / 2) < 18 && fabs(z - ZRES / 2.0) < 18) {
                //     data_arr[i] = 0xFFFFFFFF; // ABGR
                // }


                // if ((x/2 + y/2 + z/2) % 2 == 0)
                //     continue;
                // if (y % 4 != 0)
                //     continue;
                // if (util::hypot(x - XRES / 2, y - YRES / 2, z - ZRES / 2) > 60.0f)
                //     continue;
            }
        }
    }

    for (int z = 1; z < ZRES - 1; z++) {
    for (int y = 1; y < YRES - 1; y++) {
        for (int x = 1; x < XRES - 1 ; x++) {
            int i = x + y * XRES + z * YRES * XRES;

            for (int i2 = 1; i2 <= maxlod; i2++) {
                int A = 1  << i2;
                int newi = (x / A) + (y / A) * (XRES / A + 1) + (z / A) * ((YRES / A + 1) * (XRES / A + 1));
                data_lod[i2 - 1][newi] = (data_arr[i] & 0xFF000000) ? 255 : data_lod[i2 - 1][newi];
            }

            // int i2 = (x / A) + (y / A) * (XRES / A + 1) + (z / A) * ((YRES / A + 1) * (XRES / A + 1));
            // int i3 = (x / B) + (y / B) * (XRES / B + 1) + (z / B) * ((YRES / B + 1) * (XRES / B + 1));

            // lod4_arr[i2]  = (data_arr[i] & 0xFF000000) ? 255 : lod4_arr[i2];
            // lod16_arr[i3] = (data_arr[i] & 0xFF000000) ? 255 : lod16_arr[i3];
        }
    }
    }

    // TODO: GL_RGBA
    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_3D, color_tex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, XRES, YRES, ZRES, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_arr);

    for (int i = 0; i < maxlod; i++) {
        int A = 1 << (i + 1);
        glGenTextures(1, &lod[i]);
        glBindTexture(GL_TEXTURE_3D, lod[i]);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, XRES / A + 1, YRES / A + 1, ZRES / A + 1, 0, GL_RED, GL_UNSIGNED_BYTE, data_lod[i]);
    }

    // glGenTextures(1, &lod16);
    // glBindTexture(GL_TEXTURE_3D, lod16);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, XRES / B + 1, YRES / B + 1, ZRES / B + 1, 0, GL_RED, GL_UNSIGNED_BYTE, lod16_arr);


    delete[] data_arr;
    for (int i = 0; i < maxlod; i++)
        delete[] data_lod[i];
    delete[] data_lod;

    part_shader_res_loc = GetShaderLocation(part_shader, "resolution");
    part_shader_camera_pos_loc  = GetShaderLocation(part_shader, "cameraPos");
    part_shader_camera_dir_loc = GetShaderLocation(part_shader, "cameraDir");
    part_shader_sim_res_loc = GetShaderLocation(part_shader, "simRes");
    part_shader_uv1_loc =  GetShaderLocation(part_shader, "uv1");
    part_shader_uv2_loc =  GetShaderLocation(part_shader, "uv2");
}

void Renderer::draw(Simulation * sim, RenderCamera * cam) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, color_tex);

    for (int i = 0; i < maxlod; i++) {
        glActiveTexture(GL_TEXTURE0 + (i + 1));
        glBindTexture(GL_TEXTURE_3D, lod[i]);
    }


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
