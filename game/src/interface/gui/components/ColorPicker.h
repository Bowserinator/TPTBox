#ifndef INTERFACE_GUI_COMPONENTS_COLORPICKER_H_
#define INTERFACE_GUI_COMPONENTS_COLORPICKER_H_

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
            this->m_value = c;
            if (!noCallback) m_on_value_change(m_value);
        }
        Color value() const { return m_value; }

        ColorPicker * noAlpha() { m_has_alpha = false; return this; }
        ColorPicker * setOnValueChange(const std::function<void (Color)> &f) { m_on_value_change = f; return this; }
    protected:
        friend ColorPickerWindow;
        Color m_value = RED;
        bool m_has_alpha = true;
        std::function<void (Color)> m_on_value_change = [](Color){};
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_COLORPICKER_H_
