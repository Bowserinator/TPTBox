#include "Slider.h"
#include "../styles.h"

using namespace ui;

constexpr float HORIZONTAL_PAD = 10.0f;

void Slider::draw(const Vector2 &screenPos) {
    float centerY = screenPos.y + size.y / 2 - styles::SLIDER_HEIGHT / 2;
    float smallWidth = size.x - 2 * HORIZONTAL_PAD;

    DrawRectangle(screenPos.x + HORIZONTAL_PAD, centerY, smallWidth,
        styles::SLIDER_HEIGHT, styles::SCROLL_TRACK_COLOR);
    DrawRectangle(screenPos.x + HORIZONTAL_PAD, centerY, smallWidth * m_percent,
        styles::SLIDER_HEIGHT, style.backgroundColor);
    DrawCircle(screenPos.x + HORIZONTAL_PAD + smallWidth * m_percent,
        screenPos.y + size.y / 2, styles::SLIDER_THUMB_RADIUS, style.backgroundColor);
}

void Slider::onMouseMoved(Vector2 localPos) {
    InteractiveComponent::onMouseMoved(localPos);
    if (m_dragging) {
        m_percent = (localPos.x - HORIZONTAL_PAD) / (size.x - 2 * HORIZONTAL_PAD);
        _limitPercent();
        m_change_callback(m_percent);
    }
}

void Slider::onMouseClick(Vector2 localPos, unsigned button) {
    InteractiveComponent::onMouseClick(localPos, button);
    if (button == MOUSE_BUTTON_LEFT) {
        m_dragging = true;
        m_percent = (localPos.x - HORIZONTAL_PAD) / (size.x - 2 * HORIZONTAL_PAD);
        _limitPercent();
        m_change_callback(m_percent);
    }
}

void Slider::onMouseRelease(Vector2 localPos, unsigned button) {
    InteractiveComponent::onMouseRelease(localPos, button);
    if (button == MOUSE_BUTTON_LEFT)
        m_dragging = false;
}
