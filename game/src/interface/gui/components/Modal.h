#ifndef GUI_MODAL_H
#define GUI_MODAL_H

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"
#include "Panel.h"

namespace ui {
    class Modal: public Panel {
    public:
        Modal(const Vector2 &pos, const Vector2 &size, const Style &style = Style::getDefault()): Panel(pos, size, style) {}

        virtual void draw(const Vector2 &screenPos) override {
            DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, style.getBackgroundColor(this));
            DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
                style.borderThickness,
                style.getBorderColor(this));
            Panel::draw(screenPos);
        }
    };
}

#endif