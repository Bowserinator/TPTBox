#ifndef GUI_PANEL_H
#define GUI_PANEL_H

#include "raylib.h"
#include "Component.h"
#include <vector>

namespace ui {
    // A container for multiple child components
    class Panel : public Component {
    public:
        Panel(const Vector2 &pos, const Vector2 &size);
        ~Panel();

        void tick(float dt) override;
		void draw(const Vector2 &pos) override;

		void onMouseMoved(int localx, int localy) override;
		void onMouseEnter(int localx, int localy) override;
		void onMouseLeave(int localx, int localy) override;
		void onMouseDown(int x, int y, unsigned button) override;
		void onMouseUp(int x, int y, unsigned button) override;
		void onMouseClick(int localx, int localy, unsigned button) override;
		void onMouseWheel(int localx, int localy, float d) override;
		void onMouseWheelInside(int localx, int localy, float d) override;
		void onKeyDown(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) override;
		void onKeyUp(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) override;

        // @brief Add a child to the component list
        // @param component Component
        void addChild(Component * component);

        // @brief Remove a child from component list by value
        // @param component 
        void removeChild(Component * component);

        // @brief Get child by idx, is not bounds checked
        // @param idx
        // @return child obj pointer
        Component * getChildIdx(std::size_t idx);

        // @brief get number of children
        // @return number of children
        std::size_t getChildCount() const;

        // Update all controls globally
        void updateGlobalControls();

        std::vector<Component *> children;
    };
}

#endif