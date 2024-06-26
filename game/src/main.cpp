#include "raylib.h"
#include "screens.h"

#include "simulation/Simulation.h"
#include "interface/FontCache.h"
#include "interface/IconManager.h"
#include "interface/FrameTimeAvg.h"
#include "interface/EventConsumer.h"
#include "error_screen.h"

#include <GLFW/glfw3.h>

#include <omp.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

ScreenGameplay gameplay;

Screen * currentScreen = &gameplay;

constexpr int screenWidth = 1280;
constexpr int screenHeight = 800;

#ifdef DEBUG
#include <glad.h>

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam ) {
    if (id == 0x20071 || id == 0x20072) return; // Usage hint is GL_STATIC_READ will use VIDEO memory as the source for buffer object operations
    if (id == 0x501 && type == 0x824c) return; // GL_INVALID_VALUE error generated. <index> out of range. (No idea why this happens on some cards)

    fprintf(stderr, "GL CALLBACK: %s id = 0x%x, type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            id, type, severity, message);
}
#endif

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "The Powder Box");
    SetWindowMinSize(screenWidth, screenHeight);
    InitAudioDevice();
    SetExitKey(KEY_NULL);

    glEnable(GL_CLIP_DISTANCE0);

    #ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, nullptr);
    #endif

    omp_set_dynamic(false); // Don't allow dynamic scaling of num of threads
    FontCache::ref()->init();
    IconManager::ref()->init();
    currentScreen->init();


#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, TARGET_FPS, 1);
#else
    SetTargetFPS(TARGET_FPS);

    #ifndef DEBUG
    try {
    #endif
        while (!EventConsumer::ref()->shouldExit()) {
            currentScreen->update();
            BeginDrawing();
                currentScreen->draw();
            EndDrawing();
        }
    #ifndef DEBUG
    } catch(const std::exception &e) {
        drawErrorScreen(e);
    }
    #endif
#endif

    currentScreen->unload();
    FontCache::ref()->destroy();
    EventConsumer::ref()->destroy();

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
