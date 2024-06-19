#ifndef UTIL_COLORED_TEXT_H_
#define UTIL_COLORED_TEXT_H_

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "vector_op.h"

#include <string>

namespace text_format {
    // DrawTextEx, but with color escape codes
    // Note: does not handle newlines
    extern void draw_text_colored_ex(Font font, const char *text, Vector2 position, float fontSize,
        float spacing, Color tintBase);

    // Measure string size for Font
    extern Vector2 measure_text_ex(Font font, const char *text, float fontSize, float spacing);

    inline void draw_text_colored_ex(Font font, const std::string &text, Vector2 position, float fontSize,
           float spacing, Color tintBase) {
        draw_text_colored_ex(font, text.c_str(), position, fontSize, spacing, tintBase);
    }

    inline Vector2 measure_text_ex(Font font, const std::string &text, float fontSize, float spacing) {
        return measure_text_ex(font, text.c_str(), fontSize, spacing);
    }

    inline constexpr int C_ESCAPE = 20;
    inline constexpr int C_LIGHTGRAY = 1;
    inline constexpr int C_GRAY = 2;
    inline constexpr int C_DARKGRAY = 3;
    inline constexpr int C_YELLOW = 4;
    inline constexpr int C_GOLD = 5;
    inline constexpr int C_ORANGE = 6;
    inline constexpr int C_PINK = 7;
    inline constexpr int C_RED = 8;
    inline constexpr int C_MAROON = 9;
    inline constexpr int C_GREEN = 10;
    inline constexpr int C_LIME = 11;
    inline constexpr int C_DARKGREEN = 12;
    inline constexpr int C_SKYBLUE = 13;
    inline constexpr int C_BLUE = 14;
    inline constexpr int C_DARKBLUE = 15;
    inline constexpr int C_PURPLE = 16;
    inline constexpr int C_VIOLET = 17;
    inline constexpr int C_DARKPURPLE = 18;
    inline constexpr int C_BEIGE = 19;
    inline constexpr int C_BROWN = 20;
    inline constexpr int C_DARKBROWN = 21;
    inline constexpr int C_WHITE = 22;
    inline constexpr int C_BLACK = 23;
    inline constexpr int C_RESET = 24;

    inline constexpr Color C_COLORS[] = { LIGHTGRAY, GRAY, DARKGRAY, YELLOW, GOLD, ORANGE,
        PINK, Color{ 255, 0, 0, 255 } /* red */,
        MAROON, Color{ 0, 255, 0, 255 } /* green */,
        LIME, DARKGREEN, SKYBLUE, BLUE, DARKBLUE, PURPLE, VIOLET,
        DARKPURPLE, BEIGE, BROWN, DARKBROWN, WHITE, BLACK };

    inline const std::string fcode(int code) {
        char out[3] = {0};
        out[0] = C_ESCAPE;
        out[1] = code;
        return std::string{out};
    }

    inline const auto F_LIGHTGRAY = fcode(1);
    inline const auto F_GRAY = fcode(2);
    inline const auto F_DARKGRAY = fcode(3);
    inline const auto F_YELLOW = fcode(4);
    inline const auto F_GOLD = fcode(5);
    inline const auto F_ORANGE = fcode(6);
    inline const auto F_PINK = fcode(7);
    inline const auto F_RED = fcode(8);
    inline const auto F_MAROON = fcode(9);
    inline const auto F_GREEN = fcode(10);
    inline const auto F_LIME = fcode(11);
    inline const auto F_DARKGREEN = fcode(12);
    inline const auto F_SKYBLUE = fcode(13);
    inline const auto F_BLUE = fcode(14);
    inline const auto F_DARKBLUE = fcode(15);
    inline const auto F_PURPLE = fcode(16);
    inline const auto F_VIOLET = fcode(17);
    inline const auto F_DARKPURPLE = fcode(18);
    inline const auto F_BEIGE = fcode(19);
    inline const auto F_BROWN = fcode(20);
    inline const auto F_DARKBROWN = fcode(21);
    inline const auto F_WHITE = fcode(22);
    inline const auto F_BLACK = fcode(23);
    inline const auto F_RESET = fcode(24);
} // namespace text_format

#endif // UTIL_COLORED_TEXT_H_
