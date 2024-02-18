#ifndef GUI_SCROLL_PANEL_H
#define GUI_SCROLL_PANEL_H

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
                Panel(pos, size, style), internalSize(size), offsetTop(0.0f) {}

		void draw(const Vector2 &pos) override;
        void addChild(Component * component) override;

		void onMouseMoved(Vector2 localPos) override;
		void onMouseClick(Vector2 localPos, unsigned button) override;
        void onMouseRelease(Vector2 localPos, unsigned button) override;
		void onMouseWheelInside(Vector2 localPos, float d) override;

        Vector2 getInternalSize() const { return internalSize; }

    private:
        Vector2 internalSize;
        float offsetTop;
        bool dragging = false;

        void _limitScroll() {
            offsetTop = internalSize.y - size.y > 0 ?
                util::clamp(offsetTop, -(internalSize.y - size.y), 0.0f) :
                0.0f;
            eventDelta.y = -offsetTop;
        }
    };
}

#endif