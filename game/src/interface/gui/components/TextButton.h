#ifndef GUI_TEXT_BUTTON_H
#define GUI_TEXT_BUTTON_H

#include "raylib.h"
#include "Button.h"
#include "../../FontCache.h"
#include "../../../util/vector_op.h"
#include <string>

namespace ui {
    // Button that displays text inside
    class TextButton : public Button {
    public:
        TextButton(
            const Vector2 &pos,
            const Vector2 &size,
            const std::string &text,
            const Color bg_color = BLACK,
            const Color color = WHITE,
            const Color outline_color = WHITE
        ):
            ui::Button(pos, size), text(text), bg_color(bg_color),
            color(color), outline_color(outline_color) {};

        virtual ~TextButton() = default;

        void draw(const Vector2 &screenPos) override {
            if (hidden) return;

            DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, bg_color);
            DrawRectangleLines(screenPos.x, screenPos.y, size.x, size.y, outline_color);

            const Vector2 tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
            const Vector2 pad = (size - tsize) / 2.0f;
            SetTextLineSpacing(FONT_SIZE);
            DrawTextEx(FontCache::ref()->main_font, text.c_str(), screenPos + pad, FONT_SIZE, FONT_SPACING, color);
        }

    protected:
        std::string text;
        Color bg_color;
        Color color;
        Color outline_color;
    };
}

#endif