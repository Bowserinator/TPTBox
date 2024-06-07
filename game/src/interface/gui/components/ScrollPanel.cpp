#include "ScrollPanel.h"
#include "../styles.h"
#include "../../EventConsumer.h"

#include <algorithm>

using namespace ui;

void ScrollPanel::draw(const Vector2 &pos) {
    BeginScissorMode(pos.x, pos.y, size.x, size.y);
        Vector2 screenPos = Vector2{ pos.x, pos.y + m_offset_top };
        Rectangle drawingAreaRect { .x = pos.x, .y = pos.y, .width = size.x, .height = size.y };
        InteractiveComponent::draw(screenPos);

        for (auto child : m_children) {
            Rectangle childRect{ .x = screenPos.x + child->pos.x, .y = screenPos.y + child->pos.y,
                .width = child->size.x, .height = child->size.y };
            if (!child->hidden() && CheckCollisionRecs(drawingAreaRect, childRect))
                child->draw(Vector2{ screenPos.x + child->pos.x, screenPos.y + child->pos.y });
        }
    EndScissorMode();

    // Scrollbar
    if (m_internal_size.y - size.y > 0) {
        const float scrollbarHeight = size.y * (size.y / m_internal_size.y);
        const float w = styles::SCROLL_BAR_WIDTH;

        DrawRectangle(pos.x + size.x - w, pos.y, w, size.y, styles::SCROLL_TRACK_COLOR);
        DrawRectangle(pos.x + size.x - w, pos.y + (size.y - scrollbarHeight) * m_offset_top / -(m_internal_size.y - size.y),
            w, scrollbarHeight, styles::SCROLL_THUMB_COLOR);
    }
}

void ScrollPanel::addChild(Component * component) {
    Panel::addChild(component);
    m_internal_size.y = std::max(m_internal_size.y,
        component->pos.y + component->size.y + styles::SCROLL_PANEL_BOTTOM_PADDING);
}

void ScrollPanel::onMouseMoved(Vector2 localPos) {
    Panel::onMouseMoved(localPos);
    if (m_dragging) {
        m_offset_top -= EventConsumer::ref()->getMouseDelta().y * (m_internal_size.y / size.y);
        _limitScroll();
    }
}

void ScrollPanel::onMouseClick(Vector2 localPos, unsigned button) {
    Panel::onMouseClick(localPos, button);

    if (button == MOUSE_BUTTON_LEFT && localPos.x >= size.x - 1.5f * styles::SCROLL_BAR_WIDTH)
        m_dragging = true;
}

void ScrollPanel::onMouseRelease(Vector2 localPos, unsigned button) {
    Panel::onMouseRelease(localPos, button);
    if (button == MOUSE_BUTTON_LEFT)
        m_dragging = false;
}

void ScrollPanel::onMouseWheelInside(Vector2 localPos, float d) {
    Panel::onMouseWheelInside(localPos, d);
    m_offset_top += 48.0f * d;
    _limitScroll();
}
