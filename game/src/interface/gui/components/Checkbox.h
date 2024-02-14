#ifndef GUI_CHECKBOX_H
#define GUI_CHECKBOX_H

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"
#include "./abstract/InteractiveComponent.h"

namespace ui {
    class Checkbox: public InteractiveComponent {
    public:
        Checkbox(
            const Vector2 &pos,
            const Style &style = Style::getDefault()
        ): InteractiveComponent(pos, Vector2{styles::CHECKBOX_SIZE, styles::CHECKBOX_SIZE}, style, MOUSE_CURSOR_POINTING_HAND) {}

        void draw(const Vector2 &screenPos) override {
            constexpr float PAD = 5.0f;
            DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
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
        bool _checked;
        std::function<void (bool)> clickCallback = [](bool){};
    };
}

#endif