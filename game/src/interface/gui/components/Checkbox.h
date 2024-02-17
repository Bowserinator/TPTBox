#ifndef GUI_CHECKBOX_H
#define GUI_CHECKBOX_H

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"
#include "./abstract/InteractiveComponent.h"
#include "../../FontCache.h"

#include <functional>

namespace ui {
    class Checkbox: public InteractiveComponent {
    public:
        Checkbox(
            const Vector2 &pos,
            const Style &style = Style::getDefault(),
            const Vector2 &size = Vector2{styles::CHECKBOX_SIZE, styles::CHECKBOX_SIZE}
        ): InteractiveComponent(pos, size, style, MOUSE_CURSOR_POINTING_HAND) {}

        void draw(const Vector2 &screenPos) override {
            constexpr float PAD = 5.0f;
            DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, styles::CHECKBOX_SIZE, styles::CHECKBOX_SIZE },
                style.borderThickness,
                style.getBorderColor(this));
            if (_checked)
                DrawRectangle(screenPos.x + PAD, screenPos.y + PAD, styles::CHECKBOX_SIZE - 2 * PAD, styles::CHECKBOX_SIZE - 2 * PAD,
                    style.getTextColor(this));
        }

        void onMouseClick(Vector2 localPos, unsigned button) override {
            InteractiveComponent::onMouseClick(localPos, button);
            if (!disabled) {
                toggle();
                clickCallback(_checked);
            }
        }
    
        bool checked() const { return _checked; }
        void setChecked(bool checked) { _checked = checked; }
        void toggle() { _checked = !_checked; }

        Checkbox * setClickCallback(const std::function<void (bool)> &f) { clickCallback = f; return this; }
    protected:
        bool _checked = false;
        std::function<void (bool)> clickCallback = [](bool){};
    };


    // Checkbox with a label
    class LabeledCheckbox: public Checkbox {
    public:
        LabeledCheckbox(
            const Vector2 &pos,
            const Vector2 &size,
            const std::string &text,
            const Style &style = Style {
                .horizontalAlign = Style::Align::Left
            }
        ): Checkbox(pos, style, size), text(text) {
            tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
        }

        void setText(const std::string &str) {
            text = str;
            tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
        }

        void draw(const Vector2 &screenPos) override {
            Checkbox::draw(screenPos);

            const Vector2 pad = style.align(size, Vector2{ 5, 5 }, tsize);
            SetTextLineSpacing(FONT_SIZE);
            DrawTextEx(FontCache::ref()->main_font, text.c_str(),
                Vector2{screenPos.x + styles::CHECKBOX_SIZE + 8 + pad.x, screenPos.y + pad.y},
                FONT_SIZE, FONT_SPACING,
                style.getTextColor(this));
        }
    protected:
        Vector2 tsize;
        std::string text;
    };
}

#endif