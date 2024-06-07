#ifndef INTERFACE_GUI_COMPONENTS_SCROLLPANEL_H_
#define INTERFACE_GUI_COMPONENTS_SCROLLPANEL_H_

#include "raylib.h"
#include "./Panel.h"
#include "../Style.h"
#include "../../../util/math.h"

namespace ui {
    class Scene;

    // A container for multiple child components
    class ScrollPanel : public Panel {
    public:
        ScrollPanel(const Vector2 &pos, const Vector2 &size, const Style &style = Style::getDefault()):
                Panel(pos, size, style), m_internal_size(size), m_offset_top(0.0f) {}

        void draw(const Vector2 &pos) override;
        void addChild(Component * component) override;

        void onMouseMoved(Vector2 localPos) override;
        void onMouseClick(Vector2 localPos, unsigned button) override;
        void onMouseRelease(Vector2 localPos, unsigned button) override;
        void onMouseWheelInside(Vector2 localPos, float d) override;

        Vector2 getInternalSize() const { return m_internal_size; }

    protected:
        Vector2 m_internal_size;
        float m_offset_top;
        bool m_dragging = false;

        void _limitScroll() {
            m_offset_top = m_internal_size.y - size.y > 0 ?
                util::clamp(m_offset_top, -(m_internal_size.y - size.y), 0.0f) :
                0.0f;
            m_event_delta.y = -m_offset_top;
        }
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_SCROLLPANEL_H_
