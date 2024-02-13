#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "raylib.h"
#include "Component.h"
#include "../styles.h"
#include <functional>

namespace ui {
    // Base button class. Recommended to use derived button types instead
    class Button : public Component {
    public:
        Button(const Vector2 &pos, const Vector2 &size,
            const Color &bgColor = styles::DEFAULT_BG_COLOR,
            const Color &outlineColor = styles::DEFAULT_OUTLINE_COLOR,
            const Color &outlineHoverColor = styles::DEFAULT_HOVER_OUTLINE_COLOR);
        virtual ~Button() = default;

        void draw(const Vector2 &screenPos) override;
		void onMouseMoved(Vector2 localPos) override { Component::onMouseMoved(localPos); }
		void onMouseEnter(Vector2 localPos) override {
            Component::onMouseEnter(localPos);
            if (!disabled) enterCallback();
        }
		void onMouseLeave(Vector2 localPos) override { Component::onMouseLeave(localPos); }
		void onMouseDown(Vector2 localPos, unsigned button) override { Component::onMouseDown(localPos, button); }
		void onMouseUp(Vector2 localPos, unsigned button) override { Component::onMouseUp(localPos, button); }
		void onMouseClick(Vector2 localPos, unsigned button) override {
            Component::onMouseClick(localPos, button);
            if (!disabled) clickCallback();
        }

        Button * setClickCallback(const std::function<void ()> &f) { clickCallback = f; return this; }
        Button * setEnterCallback(const std::function<void ()> &f) { enterCallback = f; return this; }
    protected:
        std::function<void ()> clickCallback = [](){};
        std::function<void ()> enterCallback = [](){};

        Color bgColor = styles::DEFAULT_BG_COLOR;
        Color outlineColor = styles::DEFAULT_OUTLINE_COLOR;
        Color outlineHoverColor = styles::DEFAULT_HOVER_OUTLINE_COLOR;
    };
}

#endif