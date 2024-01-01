#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "rlgl.h"

#include "src/render/camera/camera.h"
#include "src/render/Particles.h"
#include "src/simulation/Simulation.h"

#include <algorithm>


#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif


static RenderCamera render_camera;
static Simulation sim;

static double simTime = 0.0f;
static double drawTime = 0.0f;
static double fps = 1.0f;

static int currentElementId = 1;


void ScreenGameplay::init() {
    render_camera = RenderCamera(); // Definition required
    render_camera.camera.position = Vector3{XRES / 2, 20.0f, ZRES / 2}; // Camera position
    render_camera.camera.target = Vector3{0.0f, 0.0f, 0.0f};      // Camera looking at point
    render_camera.camera.up = Vector3{0.0f, 1.0f, 0.0f};          // Camera up vector (rotation towards target)
    render_camera.camera.fovy = 45.0f;

    render_camera.setBounds(Vector3{ -3.0f * XRES, -3.0f * YRES, -3.0f * ZRES }, Vector3{ 4.0f * XRES, 4.0f * YRES, 4.0f * ZRES });

    rlEnableBackfaceCulling();
    rlEnableDepthTest();

    Image checked = GenImageChecked(2, 2, 1, 1, RED, GREEN);
    UnloadImage(checked);

    // Create a column of powder
    /*for (int x = 0; x < 50; x++)
        for (int z = 0;z < 50; z++)
            for (int y = 1; y < 90; y++)
                sim.create_part(x + 10, y, z + 10, 1);*/

}

void ScreenGameplay::update() {
    auto t = GetTime();
    // for (int x = 10; x < 100; x += 10)
    //      for (int z = 10; z < 100; z += 10)
    //      if (sim.pmap[z][90][x] == 0)
    //          sim.create_part(x, 90, z, 1);

    if (IsKeyDown(KEY_ONE))
        currentElementId = 1;
    else if (IsKeyDown(KEY_TWO))
        currentElementId = 2;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector3 forward = GetCameraForward(&render_camera.camera);
            forward = render_camera.camera.position + forward * 20.0f;
            forward.x = std::round(forward.x);
            forward.y = std::round(forward.y);
            forward.z = std::round(forward.z);

            const int S = 5;
            for (int x = forward.x; x < forward.x + S; x++)
            for (int y = forward.y; y < forward.y + S; y++)
            for (int z = forward.z; z < forward.z + S; z++)
                if (x > 0 && x < XRES -1 && y > 0 && y < YRES -1 && z > 0 && z < ZRES - 1)
                    sim.create_part(x, y, z, currentElementId);
    }

     
    sim.update();

    simTime = GetTime() - t;
}

void ScreenGameplay::draw() {
    render_camera.updateControls();
    // camera.updateViewProjMatrix();


    ClearBackground(BLACK);

    BeginMode3D(render_camera.camera);


    // TODO
    Vector3 forward = GetCameraForward(&render_camera.camera);
    forward = render_camera.camera.position + forward * 20.0f;
    forward.x = std::round(forward.x);
    forward.y = std::round(forward.y);
    forward.z = std::round(forward.z);
    DrawCubeWires(forward, 1, 1, 1, WHITE);
        


    //rlEnableWireMode();

    unsigned char red = 255;
    auto t = GetTime();

    
    DrawCubeParticle(sim, render_camera, Color{red, red, red, 255}, BLACK);
     drawTime = GetTime() - t;
    // DrawGrid(100, 1.0f);
    DrawCubeWires({XRES / 2, YRES / 2, ZRES / 2}, XRES, YRES, ZRES, WHITE);

   // rlDisableWireMode();
    EndMode3D();

    /*DrawRectangle(10, 10, 320, 93, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(10, 10, 320, 93, BLUE);

    DrawText("Free camera default controls:", 20, 20, 10, BLACK);
    DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, DARKGRAY);
    DrawText("- Mouse Wheel Pressed to Pan", 40, 60, 10, DARKGRAY);
    DrawText("- Z to zoom to (0, 0, 0)", 40, 80, 10, DARKGRAY);*/

    DrawText(TextFormat("CURRENT SIM: %f", (1.0f / simTime)), GetScreenWidth() - 270, 40, 20, GREEN);
    DrawText(TextFormat("CURRENT FPS: %f", fps), GetScreenWidth() - 270, 70, 20, GREEN);
    DrawText(TextFormat("Parts: %i", sim.maxId), GetScreenWidth() - 270, 100, 20, GREEN);

    fps = 1.0f / drawTime;
}

void ScreenGameplay::unload() {
    // unloadMesh(&mesh);
};
