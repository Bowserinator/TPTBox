#ifndef GUI_STYLES_H
#define GUI_STYLES_H

#include "raylib.h"

namespace styles {
    constexpr Vector2 ELEMENT_BUTTON_SIZE{ 60, 32 };
    constexpr float CHECKBOX_SIZE = 20.0f;
    constexpr Vector2 DROPDOWN_SIZE{ 300.0f, 28.0f };

    constexpr float SETTINGS_BUTTON_HEIGHT = 34.0f;
    constexpr float SCROLL_BAR_WIDTH = 10.0f;
    constexpr float SCROLL_PANEL_BOTTOM_PADDING = 20.0f;
    constexpr float SLIDER_HEIGHT = 3.0f;
    constexpr float SLIDER_THUMB_RADIUS = 7.0f;

    constexpr Color WINDOW_FADE_BG_COLOR{0, 0, 0, 128};
    constexpr Color WINDOW_TITLE_COLOR = BLUE;
    constexpr Color SCROLL_TRACK_COLOR{ 60, 60, 60, 255 };
    constexpr Color SCROLL_THUMB_COLOR = WHITE;
    constexpr Color SLIDER_DEFAULT_BG_COLOR = WHITE;
}

#endif