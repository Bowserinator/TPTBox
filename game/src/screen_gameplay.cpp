#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "rlgl.h"

#include "src/render/camera/camera.h"
#include "src/render/Particles.h"
#include "src/simulation/Simulation.h"

#include "src/interface/gui/HUD.h"
#include "src/interface/brush/Brush.h"
#include "src/interface/EventConsumer.h"

#include <algorithm>


#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif


static RenderCamera render_camera;
static Simulation sim;
static HUD hud(&sim, &render_camera);

static double simTime = 0.0f;
static double drawTime = 0.0f;
static double fps = 1.0f;

static int currentElementId = 1;

static float test = 0.0f; // TODO


#include "util/compute_shader.h"

void ScreenGameplay::init() {
//     char program[] =  R"###(
// #version 430 compatibility

// layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
// // layout(std430, binding=4) buffer readonly InBuff {
// //     float in_data[];
// // };
// // layout(std430, binding=5) buffer writeonly OutBuff {
// //     float out_data[];
// // };
// layout(r32f, binding = 0) uniform image3D in_data;
// layout(r32f, binding = 1) uniform image3D out_data;

// void main() {
//     ivec3 pos = ivec3(gl_GlobalInvocationID.xyz);
//     // const uint i = pos.x
//     //     + gl_NumWorkGroups.x * pos.y
//     //     + (gl_NumWorkGroups.x * gl_NumWorkGroups.y) * pos.z;
//     // const uint maxi = gl_NumWorkGroups.x * gl_NumWorkGroups.y * gl_NumWorkGroups.z;

//     // float left = (i > 0) ? in_data[i - 1] : 0.0f;
//     // float right = i < maxi - 1 ? in_data[i + 1] : 0.0f;
//     // out_data[i] = (left + in_data[i] + right) / 3.0f;
// }
// )###";
//     const int R = 200;
//     ComputeShader3D<float> s(program, ComputeShaderInput {
//         .sizex = R,
//         .sizey = R,
//         .sizez = R,
//         .flat_size = R * R* R,
//         .bind_idx1 = 4,
//         .bind_idx2 = 5
//     });
//     float * x = new float[R * R* R];
//     for (int i = 0; i < R * R* R; i++)
//         x[i] = i % 2;

//     float * r = new float[R * R* R];

//     int times = 10;
//     auto t = GetTime();
//     for (int j = 0; j < times; j++) {
//         // s.set_buff1(x);      // 0.67 us
//         s.use_and_dispatch(); // Time to dispatch and wait: 12 us
//         s.wait();

//         s.write_buff2_to(r); // 170 ms
//     }

//     std::cout << (GetTime() - t) / times << " TIME\n";

//     // for (auto &y : r)
//     //     std::cout << y << " ";
//     // std::cout << "\n";
//     delete[] x;
//     delete[] r;

    

    render_camera = RenderCamera(); // Definition required
    render_camera.camera.position = Vector3{XRES * 1.5f, YRES / 2, ZRES * 1.5f}; // Camera position
    render_camera.camera.target = Vector3{XRES / 2, YRES / 2, ZRES / 2};      // Camera looking at point
    render_camera.camera.up = Vector3{0.0f, 1.0f, 0.0f};          // Camera up vector (rotation towards target)
    render_camera.camera.fovy = 45.0f;

    hud.init();
    hud.setState(HUDState::DEBUG_MODE);


    render_camera.setBounds(Vector3{ -3.0f * XRES, -3.0f * YRES, -3.0f * ZRES }, Vector3{ 4.0f * XRES, 4.0f * YRES, 4.0f * ZRES });

    rlEnableBackfaceCulling();
    rlEnableDepthTest();

    Image checked = GenImageChecked(2, 2, 1, 1, RED, GREEN);
    UnloadImage(checked);

    // Create staircase
    // for (int x = 0; x < XRES; x++) 
    // for (int z = 0; z < ZRES; z++)
    //     sim.create_part(x, 50 + (x / (XRES / 2)), z, 4);

    // for (int x = 1; x < XRES / 2; x++) 
    // for (int z = 1; z < ZRES / 2; z++)
    // for (int y = 51; y < 70; y++)
    //     sim.create_part(x, y, z, 3);

    for (int x = 1; x < 10; x++) 
    for (int z = 1; z < 10; z++)
    for (int y = 51; y < 52; y++) {
        int i = sim.create_part(x, y, z, 5);
        const float F = 5.4f;
        sim.parts[i].vz = 1.0f;
        //sim.parts[i].vx = F * ((rand() % 100) / 100.0f - 0.5f);
        //sim.parts[i].vy = F * ((rand() % 100) / 100.0f - 0.5f);
        //sim.parts[i].vz = F * ((rand() % 100) / 100.0f - 0.5f);
    }

    for (int x = 1; x < XRES - 1; x++) 
    for (int z = 1; z < ZRES - 1; z++)
    for (int y = 1; y < 20; y++) {
        // sim.create_part(x, y, z, PT_DUST);
        sim.create_part(x, y + 1, z, PT_WATR);
    }

    // for (int x = 1; x < XRES - 1; x++) 
    // for (int z = 1; z < ZRES - 1; z++)
    // for (int y = 1; y < 20; y++) {
    //     if (x > XRES / 2 && y > 10) continue;
    //     if (x == XRES / 2)
    //         sim.create_part(x, y, z, PT_GOL);
    //     else
    //         sim.create_part(x, y, z, PT_WATR);
    // }


    // for (auto z = 6; z < AIR_ZRES / 2; z++)
    // for (auto y = 6; y < AIR_YRES / 2; y++)
    // for (auto x = 6; x < AIR_XRES / 2; x++) {
    //     sim.air.cells[z][y][x].data[PRESSURE_IDX] = 255.0f;
    // }

    // int i = sim.create_part(50, 50, 50, 5);
    // sim.parts[i].vx = 35.0f;
    // sim.parts[i].vy = 35.0f;
    // sim.parts[i].vz = 35.0f;

    // Create a column of powder
    /*for (int x = 0; x < 50; x++)
        for (int z = 0;z < 50; z++)
            for (int y = 1; y < 90; y++)
                sim.create_part(x + 10, y, z + 10, 1);*/

}

void ScreenGameplay::update() {
    sim.air.cells[AIR_XRES / 2][2][AIR_ZRES / 2].data[PRESSURE_IDX] = 512.0f;
    // for (int x = 10; x < 100; x += 10)
    //      for (int z = 10; z < 100; z += 10)
    //      if (sim.pmap[z][90][x] == 0)
    //          sim.create_part(x, 90, z, 1);

    auto t = GetTime();
    sim.update();
    simTime = GetTime() - t;
}

void ScreenGameplay::draw() {
    // TODO
    EventConsumer::ref()->reset();

    hud.update_controls();
    BrushRenderer::ref()->update(&sim, &render_camera);
    render_camera.update();

    ClearBackground(BLACK);

    BeginMode3D(render_camera.camera);


    // TODO
    Vector3 forward = GetCameraForward(&render_camera.camera);

    forward = render_camera.camera.position + forward * 20.0f;
    forward.x = std::round(forward.x);
    forward.y = std::round(forward.y);
    forward.z = std::round(forward.z);
        
    //rlEnableWireMode();

    unsigned char red = 255;
    auto t = GetTime();
    
    DrawCubeParticle(sim, render_camera, Color{red, red, red, 255}, BLACK);
    drawTime = GetTime() - t;
    // DrawGrid(100, 1.0f);

    // Visualize air
    // for (auto z = 1; z < AIR_ZRES - 1; z++)
    // for (auto y = 1; y < AIR_YRES - 1; y++)
    // for (auto x = 1; x < AIR_XRES - 1; x++) {
    //     float m = 20.0f;
    //     auto alpha = (std::max(-m, std::min(m, sim.air.cells[z][y][x].data[PRESSURE_IDX])) * 255.0f / m);

    //     if (alpha > 1) {
    //         DrawCube(Vector3{x * AIR_CELL_SIZE,y* AIR_CELL_SIZE,z* AIR_CELL_SIZE}, AIR_CELL_SIZE, AIR_CELL_SIZE, AIR_CELL_SIZE, Color { .r = 255, .g = 0, .b = 0, .a = (unsigned char)alpha}); 
    //     }
    //     else if (alpha < 1) {
    //         DrawCube(Vector3{x * AIR_CELL_SIZE,y* AIR_CELL_SIZE,z* AIR_CELL_SIZE}, AIR_CELL_SIZE, AIR_CELL_SIZE, AIR_CELL_SIZE, Color { .r = 0, .g = 0, .b = 255, .a = (unsigned char)-alpha}); 
    //     }
    // }


    DrawCubeWires({XRES / 2, YRES / 2, ZRES / 2}, XRES, YRES, ZRES, Color{ 60, 60, 60, 255 });

    // RaycastOutput out;
    // const float S = 90.0f * std::sin(test) + 30.0f;
    // sim.raycast(XRES / 2, YRES - 1, ZRES / 2,
    //     S * std::sin(test), -100.0f, S * std::cos(test), out);
    // test += 0.01f;
    // DrawCubeWires({ out.x, out.y, out.z }, 1, 1, 1, WHITE);



   // rlDisableWireMode();
    BrushRenderer::ref()->draw();
    EndMode3D();

    hud.draw(HUDData {
        .fps = (float)fps,
        .sim_fps = (float)(1.0f / simTime)
    });

    if (IsKeyDown(KEY_ONE))
        BrushRenderer::ref()->set_selected_element(1);
    else if (IsKeyDown(KEY_TWO))
        BrushRenderer::ref()->set_selected_element(2);
    else if (IsKeyDown(KEY_THREE))
        BrushRenderer::ref()->set_selected_element(3);
    else if (IsKeyDown(KEY_FOUR))
        BrushRenderer::ref()->set_selected_element(4);

    fps = 1.0f / drawTime;
}

void ScreenGameplay::unload() {
    // unloadMesh(&mesh);
};
