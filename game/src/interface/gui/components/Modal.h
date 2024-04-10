#ifndef GUI_MODAL_H
#define GUI_MODAL_H

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"
#include "Panel.h"

#include <iostream>

namespace ui {
    class Modal: public Panel {
    public:
        Modal(const Vector2 &pos, const Vector2 &size, const Style &style = Style::getDefault()):
            Panel(pos, size, Style(style).setAllBackgroundColors(BLANK)),
            backgroundColor(style.backgroundColor) {}

        virtual void draw(const Vector2 &screenPos) override {
            if (backgroundColor.a)
                DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, backgroundColor);
            DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
                style.borderThickness,
                style.getBorderColor(this));
            Panel::draw(screenPos);
        }
    private:
        const Color backgroundColor;
    };
}

#endif