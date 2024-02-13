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
		void onMouseMoved(int localx, int localy) override {}
		void onMouseEnter(int localx, int localy) override {}
		void onMouseLeave(int localx, int localy) override {}
		void onMouseDown(int x, int y, unsigned button) override {}
		void onMouseUp(int x, int y, unsigned button) override {}
		void onMouseClick(int localx, int localy, unsigned button) override {}
    };
}

#endif