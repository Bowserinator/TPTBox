#ifndef SIMULATION_SIGN_H_
#define SIMULATION_SIGN_H_

#include "SimulationDef.h"
#include "raylib.h"

#include <string>
#include <vector>

class Simulation;
class Renderer;

class Sign {
public:
    std::string text;
    coord_t x, y, z;

    Sign(const std::string &text, coord_t x, coord_t y, coord_t z);
    std::string get_display_text(const Simulation * const sim) const;
    void update(const Simulation * const sim, Renderer * renderer, const RenderTexture2D &tex, Rectangle region);
    void draw(Renderer * renderer, const RenderTexture2D &tex, Rectangle region) const;

private:
    Vector2 m_size = {0, 0};
    mutable bool m_updated = false;
};

class SimulationSigns {
public:
    void init();
    void update(const Simulation * const sim, Renderer * renderer);
    void draw(Renderer * renderer) const;

    void add_sign(); // TODO
    void remove_sign(); // TODO

private:
    constexpr inline static std::size_t MAX_SIGNS = 16;

    RenderTexture2D all_signs_tex = {0};
    std::vector<Sign> signs;
};

#endif // SIMULATION_SIGN_H_
