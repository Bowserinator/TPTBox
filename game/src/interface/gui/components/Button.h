#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "raylib.h"
#include "Component.h"

namespace ui {
    // Base button class. Recommended to use derived button types instead
    class Button : public Component {
    public:
        Button(const Vector2 &pos, const Vector2 &size);
        virtual ~Button() = default;

        void draw(const Vector2 &screenPos) override;
		void onMouseMoved(Vector2 localPos) override { Component::onMouseMoved(localPos); }
		void onMouseEnter(Vector2 localPos) override { Component::onMouseEnter(localPos); }
		void onMouseLeave(Vector2 localPos) override { Component::onMouseLeave(localPos); }
		void onMouseDown(Vector2 localPos, unsigned button) override { Component::onMouseDown(localPos, button); }
		void onMouseUp(Vector2 localPos, unsigned button) override { Component::onMouseUp(localPos, button); }
		void onMouseClick(Vector2 localPos, unsigned button) override { Component::onMouseClick(localPos, button); }
    };
}

#endif