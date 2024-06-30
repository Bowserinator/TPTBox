#ifndef SIMULATION_SIGN_H_
#define SIMULATION_SIGN_H_

#include "SimulationDef.h"
#include "raylib.h"

#include <string>
#include <vector>
#include <optional>

constexpr static float SIGN_UPSCALE = 5.0f;
constexpr static float MAX_SIGN_WIDTH = 1500.0f;
constexpr static float SIGN_HEIGHT = 60.0f;
constexpr static float SIGN_FONT_SIZE = 28.0f;
constexpr static float SIGN_PAD_X = 10.0f;

class Simulation;
class Renderer;

class Sign {
public:
    std::string text;
    coord_t x, y, z;

    Sign(const std::string &text, coord_t x, coord_t y, coord_t z);
    std::string get_display_text(const Simulation * const sim) const;

    void update(const Simulation * const sim, Renderer * renderer, const RenderTexture2D &tex, Rectangle region);
    void draw(Renderer * renderer, const RenderTexture2D &tex, Rectangle region,
        std::optional<Vector3> pos = std::nullopt, Color tint = WHITE) const;
    void set_text(const std::string &text) { this->text = text; m_updated = false; }
private:
    Vector2 m_size = {0, 0};
    mutable bool m_updated = false;
};

class SimulationSigns {
public:
    void init();
    void update(const Simulation * const sim, Renderer * renderer);
    void draw(Renderer * renderer) const;
    void draw_single_sign(Renderer * renderer, const Sign * sign,
        std::optional<Vector3> pos = std::nullopt, Color tint = WHITE);

    void add_sign(const std::string &text, coord_t x, coord_t y, coord_t z);
    void remove_sign(Sign * sign);
    void clear() { signs.clear(); }
    std::size_t sign_count() const { return signs.size(); }

    std::vector<Sign>& get_signs() { return signs; }

    constexpr inline static std::size_t MAX_SIGNS = 16;
private:
    RenderTexture2D all_signs_tex = {0};
    std::vector<Sign> signs;
};

#endif // SIMULATION_SIGN_H_
