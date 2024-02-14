#ifndef GUI_TEXT_BUTTON_H
#define GUI_TEXT_BUTTON_H

#include "raylib.h"
#include "./abstract/Button.h"
#include "../styles.h"
#include "../Style.h"
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
            const Style &style = Style::getDefault()
        ):
            ui::Button(pos, size, style), text(text)
        {
            tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
        };

        virtual ~TextButton() = default;

        void draw(const Vector2 &screenPos) override {
            InteractiveComponent::draw(screenPos);

            DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, style.getBackgroundColor(this));
            DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
                style.borderThickness,
                style.getBorderColor(this));

            const Vector2 pad = style.align(size, Vector2{ 5, 5 }, tsize);
            SetTextLineSpacing(FONT_SIZE);
            DrawTextEx(FontCache::ref()->main_font, text.c_str(), screenPos + pad, FONT_SIZE, FONT_SPACING,
                style.getTextColor(this));
        }

    protected:
        std::string text;
        Vector2 tsize;
    };
}

#endif