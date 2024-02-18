#ifndef GUI_ICON_BUTTON_H
#define GUI_ICON_BUTTON_H

#include "raylib.h"
#include "./abstract/Button.h"
#include "../styles.h"
#include "../Style.h"
#include "../../../util/vector_op.h"
#include "../../IconManager.h"

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
            ui::Button(pos, size, style), icon(icon)
        {
            setIcon(icon);
            float minDim = std::min(size.x, size.y) - 10.0f;
            tsize = Vector2{minDim, minDim};
        };

        void setIcon(guiIconName icon) {
            this->icon = icon;
        }

        void draw(const Vector2 &screenPos) override {
            InteractiveComponent::draw(screenPos);

            DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, style.getBackgroundColor(this));
            DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
                style.borderThickness,
                style.getBorderColor(this));

            const Vector2 pad = style.align(size, Vector2{ 5, 5 }, tsize);
            IconManager::ref()->draw(screenPos.x + pad.x, screenPos.y + pad.y, icon,
                style.getTextColor(this), tsize.x);
        }

    protected:
        guiIconName icon;
        Vector2 tsize;
    };
}

#endif