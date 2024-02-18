#include "ScrollPanel.h"
#include "../styles.h"

using namespace ui;

void ScrollPanel::draw(const Vector2 &pos) {
    BeginScissorMode(pos.x, pos.y, size.x, size.y);
        Vector2 screenPos = Vector2{ pos.x, pos.y + offsetTop };
        Rectangle drawingAreaRect { .x = pos.x, .y = pos.y, .width = size.x, .height = size.y };
        InteractiveComponent::draw(screenPos);

        for (auto child : children) {
            Rectangle childRect{ .x = screenPos.x + child->pos.x, .y = screenPos.y + child->pos.y,
                .width = child->size.x, .height = child->size.y };
            if (!child->getHidden() && CheckCollisionRecs(drawingAreaRect, childRect))
                child->draw(Vector2{ screenPos.x + child->pos.x, screenPos.y + child->pos.y });
        }
    EndScissorMode();

    // Scrollbar
    if (internalSize.y - size.y > 0) {
        const float scrollbarHeight = size.y * (size.y / internalSize.y);
        const float w = styles::SCROLL_BAR_WIDTH;

        DrawRectangle(pos.x + size.x - w, pos.y, w, size.y, styles::SCROLL_TRACK_COLOR);
        DrawRectangle(pos.x + size.x - w, pos.y + (size.y - scrollbarHeight) * offsetTop / -(internalSize.y - size.y),
            w, scrollbarHeight, styles::SCROLL_THUMB_COLOR);
    }
}

void ScrollPanel::addChild(Component * component) {
    Panel::addChild(component);
    internalSize.y = std::max(internalSize.y, component->pos.y + component->size.y + styles::SCROLL_PANEL_BOTTOM_PADDING);
}

void ScrollPanel::onMouseMoved(Vector2 localPos) {
    Panel::onMouseMoved(localPos);
    if (dragging) {
        offsetTop -= GetMouseDelta().y * (internalSize.y / size.y);
        _limitScroll();
    }
}

void ScrollPanel::onMouseClick(Vector2 localPos, unsigned button) {
    Panel::onMouseClick(localPos, button);

    if (button == MOUSE_BUTTON_LEFT && localPos.x >= size.x - 1.5f * styles::SCROLL_BAR_WIDTH)
        dragging = true;
}

void ScrollPanel::onMouseRelease(Vector2 localPos, unsigned button) {
    Panel::onMouseRelease(localPos, button);
    if (button == MOUSE_BUTTON_LEFT)
        dragging = false;
}

void ScrollPanel::onMouseWheelInside(Vector2 localPos, float d) {
    Panel::onMouseWheelInside(localPos, d);
    offsetTop += 48.0f * d;
    _limitScroll();
}
