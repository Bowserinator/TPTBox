#ifndef INTERFACE_GUI_COMPONENTS_ICONBUTTON_H_
#define INTERFACE_GUI_COMPONENTS_ICONBUTTON_H_

#include "raylib.h"
#include "./abstract/Button.h"
#include "../styles.h"
#include "../Style.h"
#include "../../../util/vector_op.h"
#include "../../IconManager.h"

#include <algorithm>

namespace ui {
    // Button that displays text inside
    class IconButton : public Button {
    public:
        IconButton(
            const Vector2 &pos,
            const Vector2 &size,
            const guiIconName icon,
            const Style &style = Style::getDefault()
        ):
            ui::Button(pos, size, style), m_icon(icon)
        {
            setIcon(icon);
            float minDim = std::min(size.x, size.y) - 10.0f;
            m_tsize = Vector2{minDim, minDim};
        };

        void setIcon(guiIconName icon) {
            this->m_icon = icon;
        }

        void draw(const Vector2 &screenPos) override {
            InteractiveComponent::draw(screenPos);

            DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, style.getBackgroundColor(this));
            DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
                style.borderThickness,
                style.getBorderColor(this));

            const Vector2 pad = style.align(size, Vector2{ 5, 5 }, m_tsize);
            IconManager::ref()->draw(screenPos.x + pad.x, screenPos.y + pad.y, m_icon,
                style.getTextColor(this), m_tsize.x);
        }

    protected:
        guiIconName m_icon;
        Vector2 m_tsize;
    };
} // namespace ui

#endif //INTERFACE_GUI_COMPONENTS_ICONBUTTON_H_
