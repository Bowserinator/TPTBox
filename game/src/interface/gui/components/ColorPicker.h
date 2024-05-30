#ifndef GUI_COLORPICKER_H
#define GUI_COLORPICKER_H

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"
#include "./abstract/InteractiveComponent.h"
#include "../../FontCache.h"

#include <functional>

class ColorPickerWindow;

namespace ui {
    class ColorPicker: public InteractiveComponent {
    public:
        ColorPicker(
            const Vector2 &pos,
            const Vector2 &size,
            const Style &style = Style::getDefault()
        ): InteractiveComponent(pos, size, style, MOUSE_CURSOR_POINTING_HAND) {}

        void draw(const Vector2 &screenPos) override;
        void onMouseClick(Vector2 localPos, unsigned button) override;

        void setValue(const Color c, const bool noCallback = false) {
            this->value = c;
            if (!noCallback) onValueChange(value);
        }
        Color getValue() const { return value; }

        ColorPicker * noAlpha() { hasAlpha = false; return this; }
        ColorPicker * setOnValueChange(const std::function<void (Color)> &f) { onValueChange = f; return this; }
    protected:
        friend ColorPickerWindow;
        Color value = RED;
        bool hasAlpha = true;
        std::function<void (Color)> onValueChange = [](Color){};
    };
}

#endif