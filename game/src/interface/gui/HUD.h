#ifndef HUD_H
#define HUD_H

#include "raylib.h"
#include "NavCube.h"
#include <numeric>

class RenderCamera;
class Simulation;
class FontCache;

// DEBUG is a macro so we can't use it as enum name
enum class HUDState { NORMAL, DEBUG_MODE };

constexpr int FPS_AVG_WINDOW_SIZE = 15;

struct HUDData {
    float fps;
    float sim_fps;
};

class HUD {
private:
    Simulation * sim;
    NavCube cube;
    HUDState state;

    float fps_avg[FPS_AVG_WINDOW_SIZE];
    float sim_fps_avg[FPS_AVG_WINDOW_SIZE];
    unsigned int fps_counter = 0;

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

    void init(); // called after OpenGL instance is initialized
    void draw(const HUDData &data);
    void setState(HUDState state) { this->state = state; }
};

#endif