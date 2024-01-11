#include "raylib.h"
#include "screens.h"

#include "simulation/Simulation.h"

#include <omp.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

ScreenGameplay gameplay;

Screen * currentScreen = &gameplay;

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "The Powder Box");
    SetConfigFlags(FLAG_MSAA_4X_HINT); // Enable Multi Sampling Anti Aliasing 4x (if available)
    InitAudioDevice();

    omp_set_dynamic(false); // Don't allow dynamic scaling of num of threads
    currentScreen->init();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // TODO: do this in seperate thread
        currentScreen->update();
        BeginDrawing();
            currentScreen->draw();
        EndDrawing();
    }
#endif

    currentScreen->unload();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
