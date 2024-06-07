#ifndef INTERFACE_GUI_COMPONENTS_PANEL_H_
#define INTERFACE_GUI_COMPONENTS_PANEL_H_

#include "raylib.h"
#include "./abstract/InteractiveComponent.h"
#include "../Style.h"
#include <vector>

namespace ui {
    class Scene;

    // A container for multiple child components
    class Panel : public InteractiveComponent {
    public:
        Panel(const Vector2 &pos, const Vector2 &size,
            const Style &style = (Style::getDefault()).setAllBackgroundColors(BLANK));
        virtual ~Panel();

        void tick(float dt) override;
        void draw(const Vector2 &pos) override;

        void onMouseMoved(Vector2 localPos) override;
        void onMouseEnter(Vector2 localPos) override;
        void onMouseLeave(Vector2 localPos) override;
        void onMouseDown(Vector2 localPos, unsigned button) override;
        void onMouseClick(Vector2 localPos, unsigned button) override;
        void onMouseRelease(Vector2 localPos, unsigned button) override;
        void onMouseWheel(Vector2 localPos, float d) override;
        void onMouseWheelInside(Vector2 localPos, float d) override;
        void updateKeys(bool shift, bool ctrl, bool alt) override;

        void onUnfocus() override;
        void processDeletion() override;

        // @brief Add a child to the component list
        // @param component Component
        virtual void addChild(Component * component);

        // @brief Remove a child from component list by value
        // @param component
        virtual void removeChild(Component * component);

        // @brief Remove all children
        virtual void clearChildren();

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
        Vector2 m_event_delta = Vector2{0, 0}; // Position offset for all events
        std::vector<Component *> m_children;
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_PANEL_H_
