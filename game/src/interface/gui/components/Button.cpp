#include "Button.h"

ui::Button::Button(const Vector2 &pos, const Vector2 &size, const Style &style):
    ui::Component(pos, size, style, MOUSE_CURSOR_POINTING_HAND) {}

void ui::Button::draw(const Vector2 &screenPos) {
    Component::draw(screenPos);
    DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, RED);
}