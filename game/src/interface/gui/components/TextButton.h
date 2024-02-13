#ifndef GUI_TEXT_BUTTON_H
#define GUI_TEXT_BUTTON_H

#include "raylib.h"
#include "Button.h"
#include "../styles.h"
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
            const Color bgColor = styles::DEFAULT_BG_COLOR,
            const Color color = styles::DEFAULT_TEXT_COLOR,
            const Color outlineColor = styles::DEFAULT_OUTLINE_COLOR,
            const Color outlineHoverColor = styles::DEFAULT_HOVER_OUTLINE_COLOR
        ):
            ui::Button(pos, size, bgColor, outlineColor, outlineHoverColor), text(text),
            color(color) {};

        virtual ~TextButton() = default;

        void draw(const Vector2 &screenPos) override {
            Component::draw(screenPos);
            if (hidden) return;

            DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, bgColor);
            DrawRectangleLines(screenPos.x, screenPos.y, size.x, size.y, hovered ? outlineHoverColor : outlineColor);

            const Vector2 tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
            const Vector2 pad = (size - tsize) / 2.0f;
            SetTextLineSpacing(FONT_SIZE);
            DrawTextEx(FontCache::ref()->main_font, text.c_str(), screenPos + pad, FONT_SIZE, FONT_SPACING, color);
        }

    protected:
        std::string text;
        Color color;
    };
}

#endif