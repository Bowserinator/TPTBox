#ifndef GUI_STYLES_H
#define GUI_STYLES_H

#include "raylib.h"

namespace styles {
    constexpr Vector2 ELEMENT_BUTTON_SIZE{ 60, 32 };
    constexpr Color DEFAULT_BG_COLOR = BLACK;
    constexpr Color DEFAULT_TEXT_COLOR = WHITE;
    constexpr Color DEFAULT_OUTLINE_COLOR = Color { 120, 120, 120, 255 };
    constexpr Color DEFAULT_HOVER_OUTLINE_COLOR = WHITE;

    constexpr Color DEFAULT_DISABLED_BG_COLOR = BLACK;
    constexpr Color DEFAULT_DISABLED_TEXT_COLOR = Color { 120, 120, 120, 255 };
    constexpr Color DEFAULT_DISABLED_OUTLINE_COLOR = Color { 120, 120, 120, 255 };
}

#endif