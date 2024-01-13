#include "HUD.h"
#include "../../render/camera/camera.h"
#include "../FontCache.h"
#include "../../simulation/Simulation.h"
#include "../../simulation/ElementClasses.h"
#include "../../util/str_format.h"
#include "../../util/util.h"
#include "../FontCache.h"

#include <string>

constexpr float PAD_X = 5.0f;
constexpr float PAD_Y = 7.0f;
constexpr float SPACING = -0.5f;
constexpr int RHUD_X_OFFSET = 100;

constexpr Color BLUE_TEXT{21, 145, 171, 255};

HUD::HUD(Simulation * sim, RenderCamera * cam):
    sim(sim), cube(cam), state(HUDState::NORMAL) {}

void HUD::init() {
    std::fill(&fps_avg[0], &fps_avg[FPS_AVG_WINDOW_SIZE], 0.0f);
    std::fill(&sim_fps_avg[0], &sim_fps_avg[FPS_AVG_WINDOW_SIZE], 0.0f);
    cube.init();
}

/**
 * @brief Draw text with a box behind it
 * @param text String to draw, must not contain new lines
 * @param x Top left X
 * @param y Top left Y
 * @param color Color of text
 */
void HUD::drawText(const char * text, int x, const int y, const Color color, const bool ralign) const {
    auto tsize = MeasureTextEx(FontCache::ref()->main_font, text, FONT_SIZE, SPACING);
    if (ralign)
        x -= tsize.x;

    SetTextLineSpacing(FONT_SIZE + PAD_Y);
    DrawRectangle(x - PAD_X, y - PAD_Y,
        tsize.x + PAD_X * 2,
        tsize.y + 2 * PAD_Y,
        Fade(BLACK, 0.5f));
    DrawTextEx(FontCache::ref()->main_font, text, Vector2{ (float)x, (float)y }, FONT_SIZE, SPACING, color);
}

// RAlign version, x marks right hand side
void HUD::drawTextRAlign(const char * text, const int x, const int y, const Color color) const {
    drawText(text, x, y, color, true);
}

void HUD::draw(const HUDData &data) {
    // Update the average fps
    fps_avg[fps_counter % FPS_AVG_WINDOW_SIZE] = data.fps;
    sim_fps_avg[fps_counter % FPS_AVG_WINDOW_SIZE] = data.sim_fps;
    fps_counter++;

    // Actually draw
    constexpr int OFFSET = 2 * PAD_Y + FONT_SIZE;
    const bool debug = state == HUDState::DEBUG_MODE;

    // Top left corner: FPS, [Parts, sim FPS]
    const char * text  = !debug ?
        TextFormat("FPS: %.3f", avg_fps()) :
        TextFormat("FPS: %.3f  Parts: %s", avg_fps(), util::format_commas(sim->parts_count).c_str());
    drawText(text, 20, 20, BLUE_TEXT);

    if (state == HUDState::DEBUG_MODE)
        drawText(TextFormat("Sim: %.3f  Thrd: %d", avg_sim_fps(), sim->actual_thread_count), 20, 20 + OFFSET, BLUE_TEXT);

    // Top right corner
    const int x = util::clamp(data.x, 0, XRES);
    const int y = util::clamp(data.y, 0, YRES);
    const int z = util::clamp(data.z, 0, ZRES);

    const char * air_data = TextFormat("Pressure: %.2f",
        sim->air.cells[z / AIR_CELL_SIZE][y / AIR_CELL_SIZE][x / AIR_CELL_SIZE].data[PRESSURE_IDX]);
    const char * line11 = data.idx ?
        TextFormat("%s,  %s", GetElements()[sim->parts[data.idx].type].Name.c_str(), air_data) :
        TextFormat("Empty,  %s", air_data);

    const char * pos_data = TextFormat("X: %i Y: %i Z: %i", data.x, data.y, data.z);
    const char * line12 = (data.idx && debug) ? TextFormat("#%i, %s", data.idx, pos_data) : pos_data;
    drawTextRAlign(TextFormat("%s,  %s", line11, line12), GetScreenWidth() - RHUD_X_OFFSET, 20, WHITE);

    // Additional lines if currently hovering an element
    if (data.idx) {
        const char * dcolor = sim->parts[data.idx].dcolor.as_int() ?
            TextFormat("#%08X", sim->parts[data.idx].dcolor.as_int()) : "0";
        drawTextRAlign(TextFormat("Temp: %.2f C  Life: %d, tmp1: %d, tmp2: %d, dcolor: %s",
                sim->parts[data.idx].temp,
                sim->parts[data.idx].life,
                sim->parts[data.idx].tmp1,
                sim->parts[data.idx].tmp2,
                dcolor),
            GetScreenWidth() - RHUD_X_OFFSET, 20 + OFFSET, WHITE);

        if (debug) {
            drawTextRAlign(TextFormat("VEL: %.2f, %.2f, %.2f, flag: %s",
                    sim->parts[data.idx].vx,
                    sim->parts[data.idx].vy,
                    sim->parts[data.idx].vz,
                    util::bitset_to_str<8>(sim->parts[data.idx].flag).c_str()
                ),
                GetScreenWidth() - RHUD_X_OFFSET, 20 + 2 * OFFSET, WHITE);
        }
    }

    // Draw the nav cube
    cube.update();
}
