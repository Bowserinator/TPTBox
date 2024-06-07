#ifndef INTERFACE_GUI_COMPONENTS_MODAL_H_
#define INTERFACE_GUI_COMPONENTS_MODAL_H_

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"
#include "Panel.h"
#include "../../EventConsumer.h"

#include <iostream>

namespace ui {
    class Modal: public Panel {
    public:
        Modal(const Vector2 &pos, const Vector2 &size, const Style &style = Style::getDefault()):
            Panel(pos, size, Style(style).setAllBackgroundColors(BLANK)),
            m_background_color(style.backgroundColor) {}

        void draw(const Vector2 &screenPos) override {
            if (m_background_color.a)
                DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, m_background_color);
            DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
                style.borderThickness,
                style.getBorderColor(this));
            Panel::draw(screenPos);
        }
    private:
        const Color m_background_color;
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_MODAL_H_
