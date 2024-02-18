#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"
#include "../../EventConsumer.h"

#include "ScrollPanel.h"
#include "Label.h"
#include "Modal.h"
#include "HR.h"

#include <string>

namespace ui {
    class Window: public Modal {
    public:
        struct Settings {
            float headerHeight = 40.0f;
            float bottomPadding = 0.0f;
            bool interceptEvents = true;
            Color titleColor = styles::WINDOW_TITLE_COLOR;
            std::string title = "";

            static Settings getDefault() {
                Settings ret;
                return ret;
            }
        };

        enum class CloseReason { BUTTON, ESC };

        Window(
            const Vector2 &pos,
            const Vector2 &size,
            const Settings &settings = Settings::getDefault(),
            const Style &style = Style::getDefault()):
        Modal(pos, size, style), settings(settings) {
            if (settings.headerHeight > 0.0f) {
                addChild(new Label(
                    Vector2{ 10.0f, 10.0f },
                    Vector2{ size.x, settings.headerHeight / 2.0f },
                    settings.title,
                    Style {
                        .horizontalAlign = Style::Align::Left,
                        .textColor = settings.titleColor
                    }
                ));
                addChild(new HR(Vector2{ 0, settings.headerHeight }, Vector2{ size.x, 0 }, Style::getDefault()));
            }

            panel = new ScrollPanel(
                Vector2{ 0, 40.0f },
                Vector2{ size.x, size.y - settings.bottomPadding - settings.headerHeight });
            addChild(panel);
        }

        void draw(const Vector2 &screenPos) override {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), styles::WINDOW_FADE_BG_COLOR);
            Modal::draw(screenPos);
        }

        // ESC to exit window
        virtual void updateKeys(bool shift, bool ctrl, bool alt) override {
            if (EventConsumer::ref()->isKeyPressed(KEY_ESCAPE))
                tryClose(CloseReason::ESC);
        }

        // Intercept all events
        void tick(float dt) override {
            Modal::tick(dt);
            if (settings.interceptEvents) {
                EventConsumer::ref()->consumeKeyboard();
                EventConsumer::ref()->consumeGamepad();
                EventConsumer::ref()->consumeMouse();
            }
        }

        virtual void tryClose(CloseReason reason) { close(); }
        void close() { removeFromParent(this); }
    protected:
        std::string title;
        ui::ScrollPanel * panel = nullptr;
        Settings settings;
    };
}

#endif