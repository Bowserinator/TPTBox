#include "InteractiveComponent.h"

void ui::InteractiveComponent::onMouseMoved(Vector2 localPos) {
    if (!disabled && contains(localPos)) SetMouseCursor(cursor);
    hovered = !disabled && contains(localPos);
}
void ui::InteractiveComponent::onMouseEnter(Vector2 localPos) { hovered = !disabled; }
void ui::InteractiveComponent::onMouseLeave(Vector2 localPos) { hovered = false; }
void ui::InteractiveComponent::onMouseClick(Vector2 localPos, unsigned button) { focused = !disabled; }
