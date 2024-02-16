#ifndef GUI_STYLES_H
#define GUI_STYLES_H

#include "raylib.h"

namespace styles {
    constexpr Vector2 ELEMENT_BUTTON_SIZE{ 60, 32 };
    constexpr float CHECKBOX_SIZE = 20.0f;
    constexpr Vector2 DROPDOWN_SIZE{ 300.0f, 28.0f };

    constexpr float SETTINGS_BUTTON_HEIGHT = 34.0f;

    constexpr Color WINDOW_FADE_BG_COLOR{0, 0, 0, 128};
    constexpr Color WINDOW_TITLE_COLOR = BLUE;
}

#endif