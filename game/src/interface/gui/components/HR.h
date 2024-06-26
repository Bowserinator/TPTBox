#ifndef INTERFACE_GUI_COMPONENTS_HR_H_
#define INTERFACE_GUI_COMPONENTS_HR_H_

#include "raylib.h"
#include "../Style.h"
#include "../../../util/vector_op.h"
#include "./abstract/Component.h"

namespace ui {
    class HR : public Component {
    public:
        HR(
            const Vector2 &pos,
            const Vector2 &size,
            const Style &style = Style { .borderColor = Color{255, 255, 255, 70} }
        ): Component(pos, size, style) {}

        void draw(const Vector2 &screenPos) override {
            DrawLineEx(screenPos, screenPos + size, style.borderThickness, style.getBorderColor(this));
        }
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_HR_H_
