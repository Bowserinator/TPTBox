#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"

#include "Label.h"
#include "Modal.h"
#include "HR.h"

#include <string>

namespace ui {
    class Window: public Modal {
    public:
        Window(
            const Vector2 &pos,
            const Vector2 &size,
            const std::string &title,
            const Style &style = Style::getDefault()):
        Modal(pos, size, style) {
            addChild(new Label(
                Vector2{ 10.0f, 10.0f },
                Vector2{ size.x, 20.0f },
                title,
                Style {
                    .horizontalAlign = Style::Align::Left,
                    .textColor = styles::WINDOW_TITLE_COLOR
                }
            ));
            addChild(new HR(Vector2{ 0, 40.0f }, Vector2{ size.x, 0 }, Style::getDefault()));

            panel = new Panel(Vector2{ 0, 50.0f }, Vector2{ size.x, size.y - styles::SETTINGS_BUTTON_HEIGHT - 50.0f });
            addChild(panel);
        }

        void draw(const Vector2 &screenPos) override {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), styles::WINDOW_FADE_BG_COLOR);
            Modal::draw(screenPos);
        }
    protected:
        std::string title;
        ui::Panel * panel = nullptr;
    };
}

#endif