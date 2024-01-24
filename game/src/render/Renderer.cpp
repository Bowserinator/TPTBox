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

    using T = unsigned int;
    T * data_arr = new T[ ZRES * YRES * XRES];
    memset(data_arr, 0x0, sizeof(T) * XRES * YRES * ZRES);
    for (int z = 1; z < ZRES - 1; z++) {
        for (int y = 1; y < YRES - 1; y++) {
            for (int x = 1; x < XRES - 1 ; x++) {
                int i = x + y * XRES + z * YRES * XRES;

                if (y > YRES - 3 || z < 2)
                    data_arr[i] = 0x0FFFFFFF;
                if (x < 2)
                    data_arr[i] = 0x0F0000FF;
                if (y < 2)
                    data_arr[i] = 0x0F00FF00;
                if (x > XRES - 3)
                    data_arr[i] = 0x0F00FF00;

                float s = 1/15.0;
                float x2 = (x - XRES / 2.0) * s;
                float y2 = (y - YRES / 2.0) * s;
                float z2 = (z - ZRES / 2.0) * s;
                float r = 0.5 * (x2*x2*x2*x2 + y2*y2*y2*y2 + z2*z2*z2*z2) - 8 * (x2*x2+y2*y2+z2*z2) + 60;
               // r = std::pow(4 - std::sqrt(x2*x2 + z2*z2), 2) + y2*y2 - 4;

                // if (r < 0) {
                //     data_arr[i] = 0xFF000000; // ABGR
                //     unsigned char r2 = x % 256;
                //     unsigned char g = y % 256;
                //     unsigned char b = z % 256;
                //     data_arr[i] += r2 + 256 * g + 256 * 256 * b;
                // }
                // if (std::hypot(x - XRES * 0.25, y - YRES / 2.0, z - ZRES / 2.0) < 40) {
                if (fabs(x - (float)XRES / 4) < 2 && fabs(y - (float)YRES / 2) < 20 && fabs(z - ZRES / 2.0) < 20) {
                    data_arr[i] = 0x22FF0000; // ABGR
                }
                if (fabs(x - (float)XRES / 4 - 6) < 2 && fabs(y - (float)YRES / 2) < 10 && fabs(z - ZRES / 2.0 ) < 10) {
                    data_arr[i] = 0x22FF0000; // ABGR
                }
                if (fabs(x - 3 * (float)XRES / 4) < 20 && fabs(y - (float)YRES / 2) < 20 && fabs(z - ZRES / 2.0) < 20) {
                    data_arr[i] = 0xFFFF0000; // ABGR
                }


                // if ((x/2 + y/2 + z/2) % 2 == 0)
                //     continue;
                // if (y % 4 != 0)
                //     continue;
                // if (util::hypot(x - XRES / 2, y - YRES / 2, z - ZRES / 2) > 60.0f)
                //     continue;
            }
        }
    }

    // TODO: GL_RGBA
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, XRES, YRES, ZRES, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_arr);
   //  glGenerateMipmap(GL_TEXTURE_3D);
    delete[] data_arr;

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
