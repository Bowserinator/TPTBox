#ifndef INTERFACE_GUI_COMPONENTS_WINDOW_H_
#define INTERFACE_GUI_COMPONENTS_WINDOW_H_

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
            bool draggable = false;
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
        Modal(pos, size, style), m_settings(settings) {
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

            m_panel = new ScrollPanel(
                Vector2{ 0, settings.headerHeight + 1.0f },
                Vector2{ size.x, size.y - settings.bottomPadding - settings.headerHeight - 1 });
            addChild(m_panel);
        }

        void draw(const Vector2 &screenPos) override {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), styles::WINDOW_FADE_BG_COLOR);
            Modal::draw(screenPos);
        }

        // Dragging the header
        void onMouseClick(Vector2 localPos, unsigned int button) override {
            Modal::onMouseClick(localPos, button);
            if (m_settings.draggable && button == MOUSE_BUTTON_LEFT && localPos.y <= m_settings.headerHeight) {
                m_start_drag = localPos;
                m_dragging = true;
            }
        }
        void onMouseMoved(Vector2 localPos) override {
            Modal::onMouseMoved(localPos);
            if (m_dragging) {
                Vector2 newPos = GetMousePosition() - m_start_drag;
                newPos.x = util::clamp(newPos.x, 0.0f, GetScreenWidth() - size.x);
                newPos.y = util::clamp(newPos.y, 0.0f, GetScreenHeight() - size.y);
                pos = newPos;
            }
            EventConsumer::ref()->consumeMouseDelta();
        }
        void onMouseRelease(Vector2 localPos, unsigned int button) override {
            Modal::onMouseRelease(localPos, button);
            if (button == MOUSE_BUTTON_LEFT)
                m_dragging = false;
        }

        // ESC to exit window
        void updateKeys(bool shift, bool ctrl, bool alt) override {
            if (EventConsumer::ref()->isKeyPressed(KEY_ESCAPE))
                tryClose(CloseReason::ESC);
        }

        // Intercept all events
        void tick(float dt) override {
            Modal::tick(dt);
            if (m_settings.interceptEvents) {
                EventConsumer::ref()->consumeKeyboard();
                EventConsumer::ref()->consumeGamepad();
                EventConsumer::ref()->consumeMouse();
            }
        }

        virtual void tryClose(CloseReason reason) { close(); }
        void close() { removeFromParent(this); }

    protected:
        std::string m_title;
        ui::ScrollPanel * m_panel = nullptr;
        Settings m_settings;

        bool m_dragging = false;
        Vector2 m_start_drag{0, 0};
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_WINDOW_H_
