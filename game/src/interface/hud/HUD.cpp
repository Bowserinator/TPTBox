#include "HUD.h"
#include "../../render/camera/camera.h"
#include "../EventConsumer.h"
#include "../FrameTimeAvg.h"
#include "../FontCache.h"
#include "../../simulation/Simulation.h"
#include "../../simulation/ElementClasses.h"
#include "../../util/str_format.h"
#include "../../util/math.h"
#include "../../util/colored_text.h"
#include "../brush/Brush.h"
#include "../../render/Renderer.h"
#include "../settings/data/SettingsData.h"

#include <string>
#include <cstring>
#include <algorithm>

constexpr float PAD_X = 5.0f;
constexpr float PAD_Y = 7.0f;
constexpr int RHUD_X_OFFSET = 100;

constexpr Color BLUE_TEXT{21, 145, 171, 255};

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
    const auto tsize = MeasureTextEx(FontCache::ref()->main_font, text, FONT_SIZE, FONT_SPACING);
    if (ralign)
        x -= tsize.x;

    SetTextLineSpacing(FONT_SIZE + PAD_Y);
    DrawRectangle(x - PAD_X, y - PAD_Y,
        tsize.x + PAD_X * 2,
        tsize.y + 2 * PAD_Y,
        Fade(BLACK, 0.5f));
    DrawTextEx(FontCache::ref()->main_font, text, Vector2{ (float)x, (float)y }, FONT_SIZE, FONT_SPACING, color);
}

// RAlign version, x marks right hand side
void HUD::drawTextRAlign(const char * text, const int x, const int y, const Color color) const {
    drawText(text, x, y, color, true);
}

void HUD::displayTooltip(const char * text) {
    #ifdef DEBUG
    if (strlen(text) > MAX_TOOLTIP_LENGTH)
        throw std::invalid_argument(TextFormat("Tooltip '%s' exceeds max length of %i characters", text, MAX_TOOLTIP_LENGTH));
    #endif

    tooltip_opacity = 1.0;
    strncpy(tooltip, text, sizeof(tooltip));
}

void HUD::updateControls(const BrushRenderer &brush_renderer) {
    // This updates cube controls and textures
    cube.update();

    // Rest of HUD controls
    bool consumeKey = false;
    if (EventConsumer::ref()->isKeyPressed(KEY_P)) { // Pause
        sim->set_paused(!sim->paused);
        displayTooltip(sim->paused ? "Paused" : "Unpaused");
        consumeKey = true;
    }
    if (EventConsumer::ref()->isKeyPressed(KEY_G)) { // Grid
        grid_type = (grid_type + 1) % (sizeof(GRID_VALUES) / sizeof(float));
        renderer->set_grid_size(GRID_VALUES[grid_type]);
        displayTooltip(TextFormat("Grid size: %d", (int)GRID_VALUES[grid_type]));
        consumeKey = true;
    }
    if (EventConsumer::ref()->isKeyPressed(KEY_H)) { // Cycle gravity
        sim->cycle_gravity_mode();
        displayTooltip(TextFormat("Gravity: %s", Simulation::getGravityModeName(sim->gravity_mode)));
        consumeKey = true;
    }
    if (EventConsumer::ref()->isKeyPressed(KEY_F)) { // Set rotate point
        cam->setLerpTarget(cam->camera.position, (Vector3)brush_renderer.get_raycast_pos(), cam->camera.up);
        consumeKey = true;
    }
    if (EventConsumer::ref()->isKeyPressed(KEY_F1)) // Toggle HUD
        showHUD = !showHUD;
    if (EventConsumer::ref()->isKeyPressed(KEY_D)) // Toggle debug
        state = state == HUDState::DEBUG_MODE ? HUDState::NORMAL : HUDState::DEBUG_MODE;

    // Display modes
    constexpr int NUM_KEYS[] = { KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE, KEY_ZERO };
    for (int i = 0; i < std::min(10, (int)DisplayMode::LAST); i++) {
        if (EventConsumer::ref()->isKeyPressed(NUM_KEYS[i])) {
            consumeKey = true;
            sim->graphics.set_display_mode((DisplayMode)i);
            displayTooltip(TextFormat("Display Mode: %s",
                displayModeProperties[(std::size_t)sim->graphics.display_mode].name.c_str()));
            break;
        }
    }

    if (consumeKey)
        EventConsumer::ref()->consumeKeyboard();
}

void HUD::draw(const HUDData &data) {
    // Actually draw
    constexpr int OFFSET = 2 * PAD_Y + FONT_SIZE;
    const bool debug = state == HUDState::DEBUG_MODE;

    // Handle brush tooltips
    if (data.brush_renderer->hasTooltip()) {
        displayTooltip(data.brush_renderer->getTooltip().c_str());
        data.brush_renderer->clearTooltip();
    }

    // Raycast into sim
    const Vector3T<int> raycast_pos = data.brush_renderer->get_raycast_pos();
    const int rx = raycast_pos.x;
    const int ry = raycast_pos.y;
    const int rz = raycast_pos.z;
    uint32_t idx = 0;
    if (rx >= 0 && ry >= 0 && rz >= 0) {
        idx = ID(sim->pmap[rz][ry][rx]);
        if (ID(sim->photons[rz][ry][rx]))
            idx = ID(sim->photons[rz][ry][rx]);
    }

    // Brush tooltip
    if (showHUD && debug && data.brush_renderer->brush_in_sim()) {
        const Vector3T<int> brush_pos = data.brush_renderer->get_brush_pos();
        const auto bsize = data.brush_renderer->get_size();
        drawText(TextFormat("%d, %d, %d / d%d / s%d, %d, %d",
                brush_pos.x, brush_pos.y, brush_pos.z, data.brush_renderer->get_offset(),
                bsize.x, bsize.y, bsize.z),
            GetMouseX() + 20.0f, GetMouseY() + 20.0f, WHITE);
    }

    // Bottom left: camera coords + movement mode
    if (showHUD) {
        drawText(TextFormat("Cam: %d, %d, %d %s",
                (int)std::round(cam->camera.position.x),
                (int)std::round(cam->camera.position.y),
                (int)std::round(cam->camera.position.z),
                cam->isLocked() ? "/ [Cursor Lock (RCLICK)]" : ""),
            20.0f, GetScreenHeight() - 120.0f, WHITE);

        for (int line = 0; line < 2; line++) {
            std::string text;
            if (line == 0)
                text = (IsKeyDown(KEY_LEFT_SHIFT) ? text_format::F_WHITE : text_format::F_GRAY)
                    + "[Shift: Secondary tool]";
            else if (line == 1)
                text = (IsKeyDown(KEY_LEFT_CONTROL) ? text_format::F_WHITE : text_format::F_GRAY)
                    + "[Ctrl+scroll: brush size]";

            const auto tsize = text_format::measure_text_ex(FontCache::ref()->main_font,
                text.c_str(), FONT_SIZE, FONT_SPACING);
            const float x = 20.0f;
            const float y = GetScreenHeight() - 120.0f - tsize.y - (tsize.y + 2 * PAD_Y) * (line + 1);
            DrawRectangle(x - PAD_X, y - PAD_Y,
                tsize.x + PAD_X * 2,
                tsize.y + 2 * PAD_Y,
                Fade(BLACK, 0.5f));
            text_format::draw_text_colored_ex(FontCache::ref()->main_font, text,
                Vector2{ x, y }, FONT_SIZE, FONT_SPACING, WHITE);
        }
    }

    // Update the average fps
    fps_avg[fps_counter % FPS_AVG_WINDOW_SIZE] = data.fps;
    sim_fps_avg[fps_counter % FPS_AVG_WINDOW_SIZE] = data.sim_fps;
    fps_counter++;

    if (showHUD) {
        // Top left corner: FPS, [Parts, sim FPS]
        const char * text  = !debug ?
            TextFormat("FPS: %.3f", avgFPS()) :
            TextFormat("FPS: %.3f  Parts: %s", avgFPS(), util::format_commas(sim->parts_count).c_str());
        drawText(text, 20, 20, BLUE_TEXT);

        if (state == HUDState::DEBUG_MODE)
            drawText(TextFormat("Sim: %.3f  Thrd: %d", avgSimFPS(), sim->actual_thread_count),
                20, 20 + OFFSET, BLUE_TEXT);

        // Top right corner
        const int x = util::clamp(rx, 0, XRES);
        const int y = util::clamp(ry, 0, YRES);
        const int z = util::clamp(rz, 0, ZRES);

        const char * air_data = TextFormat("Pressure: %.2f",
            sim->air.cells[z / AIR_CELL_SIZE][y / AIR_CELL_SIZE][x / AIR_CELL_SIZE].data[PRESSURE_IDX]);

        // Ctype
        std::string ctype_data = "";
        if (sim->parts[idx].ctype) {
            if (sim->parts[idx].type == PT_LAVA)
                ctype_data = " (Molten " + GetElements()[sim->parts[idx].ctype].Name + ')';
            else
                ctype_data = " (" + GetElements()[sim->parts[idx].ctype].Name + ')';
        }

        // Names
        std::string name = GetElements()[sim->parts[idx].type].Name;
        if (sim->parts[idx].type == PT_GOL)
            name = golRules[util::clamp(sim->parts[idx].tmp2 - 1, 0, GOL_RULE_COUNT - 1)].name;

        const char * line11 = idx ?
            TextFormat("%s%s,  %s", name.c_str(), ctype_data.c_str(), air_data) :
            TextFormat("Empty,  %s", air_data);

        const char * pos_data = TextFormat("X: %i Y: %i Z: %i", rx, ry, rz);
        const char * line12 = (idx && debug) ? TextFormat("#%i, %s", idx, pos_data) : pos_data;
        drawTextRAlign(TextFormat("%s,  %s", line11, line12), GetScreenWidth() - RHUD_X_OFFSET, 20, WHITE);

        // Additional lines if currently hovering an element
        if (idx) {
            const char * dcolor = sim->parts[idx].dcolor.as_RGBA() ?
                TextFormat("#%08X", sim->parts[idx].dcolor.as_RGBA()) : "0";

            float formattedTemp;
            char tempUnit;
            switch (settings::data::ref()->ui->temperatureUnit) {
                case settings::UI::TemperatureUnit::C:
                    tempUnit = 'C';
                    formattedTemp = sim->parts[idx].temp - R_ZERO_C;
                    break;
                case settings::UI::TemperatureUnit::F:
                    tempUnit = 'F';
                    formattedTemp = (sim->parts[idx].temp - R_ZERO_C) * 1.8f + 32.0f;
                    break;
                default:
                    formattedTemp = sim->parts[idx].temp;
                    tempUnit = 'K';
                    break;
            }

            drawTextRAlign(TextFormat("Temp: %.2f %c  Life: %d, tmp1: %d, tmp2: %d, dcolor: %s",
                    formattedTemp,
                    tempUnit,
                    sim->parts[idx].life,
                    sim->parts[idx].tmp1,
                    sim->parts[idx].tmp2,
                    dcolor),
                GetScreenWidth() - RHUD_X_OFFSET, 20 + OFFSET, WHITE);

            if (debug) {
                drawTextRAlign(TextFormat("VEL: %.2f, %.2f, %.2f, flag: %s",
                        sim->parts[idx].vx,
                        sim->parts[idx].vy,
                        sim->parts[idx].vz,
                        sim->parts[idx].flag.to_string().c_str()
                    ),
                    GetScreenWidth() - RHUD_X_OFFSET, 20 + 2 * OFFSET, WHITE);
            }
        }

        // Draw the nav cube
        cube.draw();
    }

    // Tooltip
    if (tooltip_opacity) {
        const auto tsize = MeasureTextEx(FontCache::ref()->main_font, tooltip, FONT_SIZE, FONT_SPACING);
        DrawTextEx(FontCache::ref()->main_font, tooltip,
            Vector2{ (float)(GetScreenWidth() / 2 - tsize.x / 2), (float)(GetScreenHeight() / 2) },
            FONT_SIZE, FONT_SPACING, Color{ 255, 255, 255, (uint8_t)(tooltip_opacity * 255) });

        tooltip_opacity -= FrameTime::ref()->getDelta() / TOOLTIP_TIME_SECONDS;
        if (tooltip_opacity < 0.01)
            tooltip_opacity = 0.0;
    }
}
