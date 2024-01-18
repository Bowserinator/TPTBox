#ifndef HUD_H
#define HUD_H

#include "raylib.h"
#include "NavCube.h"
#include "../../util/vector_op.h"

#include <numeric>

class RenderCamera;
class Simulation;
class FontCache;
class BrushRenderer;

// DEBUG is a macro so we can't use it as enum name
enum class HUDState { NORMAL, DEBUG_MODE };

constexpr int FPS_AVG_WINDOW_SIZE = 15;
constexpr int MAX_TOOLTIP_LENGTH = 128;

struct HUDData {
    float fps;
    float sim_fps;
    BrushRenderer * brush_renderer;
};

class HUD {
private:
    Simulation * sim;
    NavCube cube;
    HUDState state;

    float fps_avg[FPS_AVG_WINDOW_SIZE];
    float sim_fps_avg[FPS_AVG_WINDOW_SIZE];
    unsigned int fps_counter = 0;

    char tooltip[MAX_TOOLTIP_LENGTH];
    double tooltip_opacity = 0.0f;

    float avg_fps() const {
        return std::accumulate(fps_avg, fps_avg + FPS_AVG_WINDOW_SIZE, 0.0f) / FPS_AVG_WINDOW_SIZE;
    }
    float avg_sim_fps() const {
        return std::accumulate(sim_fps_avg, sim_fps_avg + FPS_AVG_WINDOW_SIZE, 0.0f) / FPS_AVG_WINDOW_SIZE;
    }
public:
    HUD(Simulation * sim, RenderCamera * cam);

    void drawText(const char * text, int x, const int y, const Color color, const bool ralign = false) const;
    void drawTextRAlign(const char * text, const int x, const int y, const Color color) const;
    void displayTooltip(const char * text);

    void init(); // called after OpenGL instance is initialized
    void update_controls();
    void draw(const HUDData &data);
    void setState(HUDState state) { this->state = state; }
};

#endif