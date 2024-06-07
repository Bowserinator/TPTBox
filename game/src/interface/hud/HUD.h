#ifndef INTERFACE_HUD_HUD_H_
#define INTERFACE_HUD_HUD_H_

#include "raylib.h"
#include "NavCube.h"
#include "../../util/vector_op.h"

#include <numeric>

class RenderCamera;
class Simulation;
class FontCache;
class BrushRenderer;
class Renderer;

// DEBUG is a macro so we can't use it as enum name
enum class HUDState { NORMAL, DEBUG_MODE };

constexpr int FPS_AVG_WINDOW_SIZE = 15;
constexpr int MAX_TOOLTIP_LENGTH = 128;
constexpr double TOOLTIP_TIME_SECONDS = 0.7;
constexpr float GRID_VALUES[] = {0.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f};

struct HUDData {
    float fps;
    float sim_fps;
    BrushRenderer * brush_renderer;
};

class HUD {
private:
    Simulation * sim;
    RenderCamera * cam;
    Renderer * renderer;
    NavCube cube;
    HUDState state;

    float fps_avg[FPS_AVG_WINDOW_SIZE];
    float sim_fps_avg[FPS_AVG_WINDOW_SIZE];
    unsigned int fps_counter = 0;
    unsigned int grid_type = 0;
    bool showHUD = true;

    char tooltip[MAX_TOOLTIP_LENGTH];
    double tooltip_opacity = 0.0f;

    float avgFPS() const {
        return std::accumulate(fps_avg, fps_avg + FPS_AVG_WINDOW_SIZE, 0.0f) / FPS_AVG_WINDOW_SIZE;
    }
    float avgSimFPS() const {
        return std::accumulate(sim_fps_avg, sim_fps_avg + FPS_AVG_WINDOW_SIZE, 0.0f) / FPS_AVG_WINDOW_SIZE;
    }

public:
    HUD(Simulation * sim, RenderCamera * cam, Renderer * renderer):
        sim(sim), cam(cam), renderer(renderer), cube(cam), state(HUDState::NORMAL) {}

    void drawText(const char * text, int x, const int y, const Color color, const bool ralign = false) const;
    void drawTextRAlign(const char * text, const int x, const int y, const Color color) const;
    void displayTooltip(const char * text);

    void init(); // called after OpenGL instance is initialized
    void updateControls(const BrushRenderer &brush_renderer);
    void draw(const HUDData &data);
    void setState(HUDState state) { this->state = state; }
};

#endif // INTERFACE_HUD_HUD_H_
