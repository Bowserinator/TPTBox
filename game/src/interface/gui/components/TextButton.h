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
            const Color outlineHoverColor = styles::DEFAULT_HOVER_OUTLINE_COLOR,
            const float outlineThickness = 1.0f
        ):
            ui::Button(pos, size, bgColor, outlineColor, outlineHoverColor), text(text),
            color(color), outlineThickness(outlineThickness) {};

        virtual ~TextButton() = default;

        void draw(const Vector2 &screenPos) override {
            Component::draw(screenPos);
            if (hidden) return;

            auto currentOutlineColor = disabled ?
                styles::DEFAULT_DISABLED_OUTLINE_COLOR :
                (hovered ? outlineHoverColor : outlineColor);

            DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, disabled ? styles::DEFAULT_DISABLED_BG_COLOR : bgColor);
            DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y }, outlineThickness,
                currentOutlineColor);

            const Vector2 tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
            const Vector2 pad = (size - tsize) / 2.0f;
            SetTextLineSpacing(FONT_SIZE);
            DrawTextEx(FontCache::ref()->main_font, text.c_str(), screenPos + pad, FONT_SIZE, FONT_SPACING,
                disabled ? styles::DEFAULT_DISABLED_TEXT_COLOR : color);
        }

    protected:
        std::string text;
        Color color;
        float outlineThickness;
    };
}

#endif