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

		void onMouseMoved(Vector2 localPos) override;
		void onMouseEnter(Vector2 localPos) override;
		void onMouseLeave(Vector2 localPos) override;
		void onMouseDown(Vector2 localPos, unsigned button) override;
		void onMouseUp(Vector2 localPos, unsigned button) override;
		void onMouseClick(Vector2 localPos, unsigned button) override;
		void onMouseWheel(Vector2 localPos, float d) override;
		void onMouseWheelInside(Vector2 localPos, float d) override;
		void updateKeys(bool shift, bool ctrl, bool alt) override;

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

    private:
        std::vector<Component *> children;
    };
}

#endif