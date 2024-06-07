#ifndef INTERFACE_GUI_COMPONENTS_ABSTRACT_BUTTON_H_
#define INTERFACE_GUI_COMPONENTS_ABSTRACT_BUTTON_H_

#include "raylib.h"
#include "InteractiveComponent.h"
#include "../../Style.h"
#include "../../styles.h"
#include <functional>

namespace ui {
    // Base button class. Recommended to use derived button types instead
    class Button : public InteractiveComponent {
    public:
        Button(const Vector2 &pos, const Vector2 &size, const Style &style = Style::getDefault());
        virtual ~Button() = default;

        void draw(const Vector2 &screenPos) override;
        void onMouseEnter(Vector2 localPos) override {
            InteractiveComponent::onMouseEnter(localPos);
            if (!m_disabled) m_enter_callback();
        }
        void onMouseClick(Vector2 localPos, unsigned button) override {
            InteractiveComponent::onMouseClick(localPos, button);
            if (!m_disabled) m_click_callback(button);
        }

        Button * setClickCallback(const std::function<void (unsigned int)> &f) { m_click_callback = f; return this; }
        Button * setEnterCallback(const std::function<void ()> &f) { m_enter_callback = f; return this; }
    protected:
        std::function<void (unsigned int)> m_click_callback = [](unsigned int){};
        std::function<void ()> m_enter_callback = [](){};
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_ABSTRACT_BUTTON_H_
