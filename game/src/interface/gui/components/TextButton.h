#ifndef INTERFACE_GUI_COMPONENTS_TEXTBUTTON_H_
#define INTERFACE_GUI_COMPONENTS_TEXTBUTTON_H_

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
            ui::Button(pos, size, style), m_text(text)
        {
            m_tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
        };

        virtual ~TextButton() = default;

        void setText(const std::string &text) {
            this->m_text = text;
            m_tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
        }

        void draw(const Vector2 &screenPos) override {
            InteractiveComponent::draw(screenPos);

            DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, style.getBackgroundColor(this));
            DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
                style.borderThickness,
                style.getBorderColor(this));

            const Vector2 pad = style.align(size, Vector2{ 5, 5 }, m_tsize);
            SetTextLineSpacing(FONT_SIZE);
            DrawTextEx(FontCache::ref()->main_font, m_text.c_str(), screenPos + pad, FONT_SIZE, FONT_SPACING,
                style.getTextColor(this));
        }

    protected:
        std::string m_text;
        Vector2 m_tsize;
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_TEXTBUTTON_H_
