#ifndef HUD_H
#define HUD_H

#include "raylib.h"

class Simulation;

// DEBUG is a macro so we can't use it as enum name
enum class HUDState { NORMAL, DEBUG_MODE };

struct HUDData {
    float fps;
    float sim_fps;
    int idx;
    int x, y, z;
};

class HUD {
private:
    Simulation * sim;
    HUDState state;
    Font font;

    void drawText(const char * text, int x, const int y, const Color color, const bool ralign = false) const;
    void drawTextRAlign(const char * text, const int x, const int y, const Color color) const;
public:
    HUD(Simulation * sim);

    void init();
    void draw(const HUDData &data) const;
    void setState(HUDState state) { this->state = state; }
};

#endif