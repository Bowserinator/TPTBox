#ifndef GUI_MODAL_H
#define GUI_MODAL_H

#include "raylib.h"
#include "../styles.h"
#include "Panel.h"

namespace ui {
    class Modal: public Panel {
    public:
        Modal(const Vector2 &pos, const Vector2 &size,
            const Color &bgColor = styles::DEFAULT_BG_COLOR,
            const Color &outlineColor = styles::DEFAULT_OUTLINE_COLOR
        ): Panel(pos, size), bgColor(bgColor), outlineColor(outlineColor) {}

        void draw(const Vector2 &screenPos) override {
            DrawRectangle(pos.x, pos.y, size.x, size.y, bgColor);
            DrawRectangleLines(pos.x, pos.y, size.x, size.y, outlineColor);
            Panel::draw(screenPos);
        }
    
    protected:
        Color bgColor, outlineColor;
    };
}

#endif