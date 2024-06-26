#ifndef INTERFACE_GUI_COMPONENTS_ABSTRACT_INTERACTIVECOMPONENT_H_
#define INTERFACE_GUI_COMPONENTS_ABSTRACT_INTERACTIVECOMPONENT_H_

#include "raylib.h"
#include "Component.h"
#include "../../Style.h"

namespace ui {
    // This component automatically updates hover on mouseEnter / move / leave
    // and focus on click, respecting the disabled state
    class InteractiveComponent : public Component {
    public:
        InteractiveComponent(
            const Vector2 &pos, const Vector2 &size,
            Style style = Style::getDefault(),
            const int cursor = MOUSE_CURSOR_DEFAULT
        ):
            Component(pos, size, style, cursor) {};
        virtual ~InteractiveComponent() = default;

        void onMouseMoved(Vector2 localPos) override;
        void onMouseEnter(Vector2 localPos) override;
        void onMouseLeave(Vector2 localPos) override;
        void onMouseClick(Vector2 localPos, unsigned button) override;
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_ABSTRACT_INTERACTIVECOMPONENT_H_
