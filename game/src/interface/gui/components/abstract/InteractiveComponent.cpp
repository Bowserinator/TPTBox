#include "InteractiveComponent.h"

void ui::InteractiveComponent::onMouseMoved(Vector2 localPos) {
    if (!m_disabled && contains(localPos)) SetMouseCursor(cursor);
    m_hovered = !m_disabled && contains(localPos);
}
void ui::InteractiveComponent::onMouseEnter(Vector2 localPos) { m_hovered = !m_disabled; }
void ui::InteractiveComponent::onMouseLeave(Vector2 localPos) { m_hovered = false; }
void ui::InteractiveComponent::onMouseClick(Vector2 localPos, unsigned button) { m_focused = !m_disabled; }
