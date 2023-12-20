#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "src/render/Camera.h"
#include "src/render/Particles.h"
#include "src/simulation/Simulation.h"

#include <algorithm>

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

static Camera3D camera = {0};
static Simulation sim;

static double simTime = 0.0f;

// Logo Screen Initialization logic
void InitLogoScreen(void)
{
    camera.position = Vector3{XRES / 2, 20.0f, ZRES / 2}; // Camera position
    camera.target = Vector3{0.0f, 0.0f, 0.0f};      // Camera looking at point
    camera.up = Vector3{0.0f, 1.0f, 0.0f};          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;

    // Create a column of powder
    /*for (int x = 0; x < 10; x++)
        for (int z = 0;z < 10; z++)
            for (int y = 0; y < 90; y++)
                sim.create_part(x + 10, y, z + 10, 1);*/

}

// Logo Screen Update logic
void UpdateLogoScreen(void)
{
    auto t = GetTime();
    for (int x = 10; x < 100; x += 10)
        for (int z = 10; z < 100; z += 10)
        if (sim.pmap[z][90][x] == 0)
            sim.create_part(x, 90, z, 1);
    
    sim.update();

    simTime = GetTime() - t;
}

// Logo Screen Draw logic
void DrawLogoScreen(void)
{
    UpdateSimCamera(&camera);
    BeginDrawing();

    ClearBackground(BLACK);

    BeginMode3D(camera);

    // for (int i = 0; i <= sim.maxId; i++)
    // {
    //     const auto &part = sim.parts[i];
    //     if (part.id == 0) continue;
    //     Vector3 cubePosition = {(int)(part.x + 0.5f), (int)(part.y + 0.5f), (int)(part.z + 0.5f)};
    //     char red = 255; // std::max(0, 255 - (int)part.y * 50);
    //     DrawCubeParticle(cubePosition, Color{red, red, red, 255}, BLACK);
    // }
    char red = 255;
    DrawCubeParticle(sim, Color{red, red, red, 255}, BLACK);

    // DrawGrid(100, 1.0f);
    DrawCubeWires({XRES / 2, YRES / 2, ZRES / 2}, XRES, YRES, ZRES, WHITE);

    EndMode3D();

    /*DrawRectangle(10, 10, 320, 93, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(10, 10, 320, 93, BLUE);

    DrawText("Free camera default controls:", 20, 20, 10, BLACK);
    DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, DARKGRAY);
    DrawText("- Mouse Wheel Pressed to Pan", 40, 60, 10, DARKGRAY);
    DrawText("- Z to zoom to (0, 0, 0)", 40, 80, 10, DARKGRAY);*/

    DrawText(TextFormat("CURRENT FPS: %f", (1.0f / simTime)), GetScreenWidth() - 220, 40, 20, GREEN);
    DrawText(TextFormat("Parts: %i", sim.maxId), GetScreenWidth() - 220, 90, 20, GREEN);

    EndDrawing();

}

// Logo Screen Unload logic
void UnloadLogoScreen(void)
{
    // Unload LOGO screen variables here!
}

// Logo Screen should finish?
int FinishLogoScreen(void)
{
    return 0;
}
