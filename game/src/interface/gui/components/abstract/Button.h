#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

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
            if (!disabled) enterCallback();
        }
		void onMouseClick(Vector2 localPos, unsigned button) override {
            InteractiveComponent::onMouseClick(localPos, button);
            if (!disabled) clickCallback();
        }

        Button * setClickCallback(const std::function<void ()> &f) { clickCallback = f; return this; }
        Button * setEnterCallback(const std::function<void ()> &f) { enterCallback = f; return this; }
    protected:
        std::function<void ()> clickCallback = [](){};
        std::function<void ()> enterCallback = [](){};
    };
}

#endif