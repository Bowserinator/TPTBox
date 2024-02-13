#include "Button.h"

ui::Button::Button(const Vector2 &pos, const Vector2 &size,
            const Color &bgColor,
            const Color &outlineColor,
            const Color &outlineHoverColor):
    ui::Component(pos, size, MOUSE_CURSOR_POINTING_HAND),
    bgColor(bgColor),
    outlineColor(outlineColor),
    outlineHoverColor(outlineHoverColor) {}

void ui::Button::draw(const Vector2 &screenPos) {
    Component::draw(screenPos);
    DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, RED);
}