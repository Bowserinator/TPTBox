#include "Sign.h"
#include "Simulation.h"
#include "ElementClasses.h"
#include "../render/Renderer.h"
#include "../render/camera/camera.h"
#include "../interface/FontCache.h"

#include <format>

constexpr static float SIGN_UPSCALE = 5.0f;
constexpr static float MAX_SIGN_WIDTH = 1500.0f;
constexpr static float SIGN_HEIGHT = 60.0f;
constexpr static float SIGN_FONT_SIZE = 28.0f;
constexpr static float PAD_X = 10.0f;

Sign::Sign(const std::string &text, coord_t x, coord_t y, coord_t z):
    text(text), x(x), y(y), z(z) {}

std::string Sign::get_display_text(const Simulation * const sim) const {
    auto ret = text;
    std::size_t i = 0;
    while (true) {
        std::size_t start = ret.find('{', i);
        if (start == std::string::npos) break;
        std::size_t end   = ret.find('}', start);
        if (end == std::string::npos) break;

        m_updated = false;
        std::string_view between_braces{ ret.begin() + start, ret.begin() + end + 1 };
        std::string replace_with = "{?}";

        part_id id = ID(sim->photons[z][y][x]);
        if (!id) id = ID(sim->pmap[z][y][x]);
        part_type ptype = TYP(sim->photons[z][y][x]);
        if (!ptype) ptype = TYP(sim->pmap[z][y][x]);

        if (between_braces == "{t}" || between_braces == "{temp}")
            replace_with = std::format("{:.2f}", sim->parts[id].temp);
        else if (between_braces == "{p}" || between_braces == "{pressure}")
            replace_with = "PRESSURE"; // TODO
        else if (between_braces == "{type}")
            replace_with = GetElements()[ptype].Name;
        else if (between_braces == "{tmp}" || between_braces == "{tmp1}")
            replace_with = std::to_string(sim->parts[id].tmp1);
        else if (between_braces == "{tmp2}")
            replace_with = std::to_string(sim->parts[id].tmp2);
        else if (between_braces == "{life}")
            replace_with = std::to_string(sim->parts[id].life);
        else if (between_braces == "{vx}")
            replace_with = std::to_string(sim->parts[id].vx);
        else if (between_braces == "{vy}")
            replace_with = std::to_string(sim->parts[id].vy);
        else if (between_braces == "{vz}")
            replace_with = std::to_string(sim->parts[id].vz);
        else if (between_braces == "{ctype}")
            replace_with = sim->parts[id].ctype <= ELEMENT_COUNT ?
                GetElements()[sim->parts[id].ctype].Name :
                std::to_string(sim->parts[id].ctype);

        ret = ret.replace(start, end - start + 1, replace_with);
        i = start + replace_with.length() - (int)(end - start);
    }

    return ret;
}

void Sign::update(const Simulation * const sim, Renderer * renderer, const RenderTexture2D &tex, Rectangle region) {
    const std::string display_text = get_display_text(sim);

    if (!m_updated) {
        m_updated = true;
        m_size = MeasureTextEx(FontCache::ref()->main_font, display_text.c_str(), SIGN_FONT_SIZE, 0.0f);

        BeginTextureMode(tex);
            DrawRectangle(region.x, region.y, m_size.x + 2 * PAD_X, region.height, Color{ 25, 25, 25, 255 });
            DrawTextEx(FontCache::ref()->main_font, display_text.c_str(),
                Vector2{ region.x + PAD_X, region.y + (region.height - m_size.y) / 2 },
                SIGN_FONT_SIZE, 0.0f, WHITE);
        EndTextureMode();
    }
}

void Sign::draw(Renderer * renderer, const RenderTexture2D &tex, Rectangle region) const {
    region.width = m_size.x + 2 * PAD_X;
    DrawBillboardRec(renderer->get_cam()->camera, tex.texture, region,
        Vector3{(float)x, (float)y, (float)z},
        Vector2{ SIGN_UPSCALE, SIGN_UPSCALE },
        WHITE);
}

// -----------
void SimulationSigns::init() {
    all_signs_tex = LoadRenderTexture(MAX_SIGN_WIDTH, MAX_SIGNS * SIGN_HEIGHT);

    signs.emplace_back("hello {t} {p} {type} {tmp} {tmp2} {ctype}", 50, 100, 50);
    signs.emplace_back("this is a sign", 50, 150, 50);
}

void SimulationSigns::add_sign() {
    // TODO
}

void SimulationSigns::remove_sign() {
    // TODO
}

void SimulationSigns::update(const Simulation * const sim, Renderer * renderer) {
    int y = 0;
    for (auto &sign : signs) {
        sign.update(sim, renderer, all_signs_tex, Rectangle { 0, y * SIGN_HEIGHT, MAX_SIGN_WIDTH, SIGN_HEIGHT });
        y++;
    }
}

void SimulationSigns::draw(Renderer * renderer) const {
    int y = 0;
    for (const auto &sign : signs) {
        // Note: height is negative because of opengl indexing, different from update
        sign.draw(renderer, all_signs_tex, Rectangle { 0, -y * SIGN_HEIGHT, MAX_SIGN_WIDTH, -SIGN_HEIGHT });
        y++;
    }
}
