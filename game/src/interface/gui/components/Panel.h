#ifndef GUI_PANEL_H
#define GUI_PANEL_H

#include "raylib.h"
#include "./abstract/InteractiveComponent.h"
#include "../Style.h"
#include <vector>

namespace ui {
    class Scene;

    // A container for multiple child components
    class Panel : public InteractiveComponent {
    public:
        Panel(const Vector2 &pos, const Vector2 &size, const Style &style = Style::getDefault());
        virtual ~Panel();

        void tick(float dt) override;
		void draw(const Vector2 &pos) override;

		virtual void onMouseMoved(Vector2 localPos) override;
		virtual void onMouseEnter(Vector2 localPos) override;
		virtual void onMouseLeave(Vector2 localPos) override;
		virtual void onMouseDown(Vector2 localPos, unsigned button) override;
		virtual void onMouseClick(Vector2 localPos, unsigned button) override;
        virtual void onMouseRelease(Vector2 localPos, unsigned button) override;
		virtual void onMouseWheel(Vector2 localPos, float d) override;
		virtual void onMouseWheelInside(Vector2 localPos, float d) override;
		virtual void updateKeys(bool shift, bool ctrl, bool alt) override;

        virtual void onUnfocus() override;
        virtual void processDeletion() override;

        // @brief Add a child to the component list
        // @param component Component
        virtual void addChild(Component * component);

        // @brief Remove a child from component list by value
        // @param component 
        virtual void removeChild(Component * component);

        // Update parent scene
        // @param scene
        void setParentScene(Scene * scene) override;

        // @brief Get child by idx, is not bounds checked
        // @param idx
        // @return child obj pointer
        Component * getChildIdx(std::size_t idx);

        // @brief get number of children
        // @return number of children
        std::size_t getChildCount() const;

    protected:
        Vector2 eventDelta = Vector2{0, 0}; // Position offset for all events
        std::vector<Component *> children;
    };
}

#endif