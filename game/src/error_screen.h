#ifndef ERROR_SCREEN_H_
#define ERROR_SCREEN_H_

#include "raylib.h"
#include "interface/FontCache.h"

#include <exception>
#include <string>

inline void drawErrorScreen(const std::exception &e) {
    BeginDrawing();
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{255, 0, 0, 150});

    constexpr float PAD = 50.0f;
    constexpr float SPACING = 10.0f;
    constexpr float FONT_SIZE = 10.0f;
    const std::string ISSUE_URL = "https://github.com/Bowserinator/TPTBox/issues";

    DrawTextEx(FontCache::ref()->main_font, "A fatal exception has occurred:",
        Vector2{PAD, PAD}, FONT_SIZE, 0.0f, WHITE);
    DrawTextEx(FontCache::ref()->main_font, "Please report the error below and how to reproduce the bug",
        Vector2{PAD, PAD + (FONT_SIZE + SPACING) * 2}, FONT_SIZE, 0.0f, WHITE);
    DrawTextEx(FontCache::ref()->main_font, ("to our Github at [" + ISSUE_URL + "]").c_str(),
        Vector2{PAD, PAD + (FONT_SIZE + SPACING) * 3}, FONT_SIZE, 0.0f, WHITE);

    const std::string prefix = "what(): ";
    DrawTextEx(FontCache::ref()->main_font, (prefix + e.what()).c_str(),
        Vector2{PAD, PAD + (FONT_SIZE + SPACING) * 5}, FONT_SIZE, 0.0f, WHITE);
    EndDrawing();

    while (!WindowShouldClose()) {
        BeginDrawing();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            OpenURL(ISSUE_URL.c_str());
        EndDrawing();
    }
}

#endif // ERROR_SCREEN_H_
