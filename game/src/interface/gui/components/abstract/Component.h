#ifndef INTERFACE_GUI_COMPONENTS_ABSTRACT_COMPONENT_H_
#define INTERFACE_GUI_COMPONENTS_ABSTRACT_COMPONENT_H_

#include "raylib.h"
#include "../../Style.h"

namespace ui {
    class Scene;

    class Component {
    public:
        Component(
            const Vector2 &pos, const Vector2 &size,
            Style style = Style::getDefault(),
            const int cursor = MOUSE_CURSOR_DEFAULT
        ):
            pos(pos), size(size), style(style), cursor(cursor) {};
        virtual ~Component() = default;

        virtual void tick(float dt);
        virtual void draw(const Vector2 &screenPos);

        virtual void onMouseMoved(Vector2 localPos);
        virtual void onMouseEnter(Vector2 localPos);
        virtual void onMouseLeave(Vector2 localPos);
        virtual void onMouseDown(Vector2 localPos, unsigned button);
        virtual void onMouseRelease(Vector2 localPos, unsigned button);
        virtual void onMouseClick(Vector2 localPos, unsigned button);
        virtual void onMouseWheel(Vector2 localPos, float d);
        virtual void onMouseWheelInside(Vector2 localPos, float d);
        virtual void updateKeys(bool shift, bool ctrl, bool alt);

        virtual void onFocus();
        virtual void onUnfocus();

        virtual void setParent(Component * parent) { this->parent = parent; }
        virtual void setParentScene(Scene * scene) { this->parentScene = scene; }

        // Add component to parent scene or Panel (whichever is nearest parent)
        void addToParent(Component * child);

        // Remove a given component from the first parent scene or Panel of this
        // Note: does not remove immediately!
        void removeFromParent(Component * child);

        // Do cleanup, reserved function (deletes immediately)
        virtual void processDeletion();

        // @param pos Local position inside the element
        inline bool contains(const Vector2 pos) const {
            return (pos.x >= 0 && pos.y >= 0 && pos.x <= size.x && pos.y <= size.y);
        }
        inline bool contains(Component * other) const {
            return ((pos.x < (other->pos.x + other->size.x) && (pos.x + size.x) > other->pos.x) &&
                (pos.y < (other->pos.y + other->size.y) && (pos.y + size.y) > other->pos.y));
        }

        Vector2 pos, size;
        Vector2 globalPos{0, 0};
        Component * parent = nullptr;
        Scene * parentScene = nullptr;

        Component * disable() { m_disabled = true; return this; }
        Component * enable() { m_disabled = false; return this; }
        Component * hide() { m_hidden = true; return this; }
        Component * show() { m_hidden = false; return this; }
        Component * focus() { m_focused = true; onFocus(); return this; }
        Component * unfocus() { m_focused = false; onUnfocus(); return this; }

        bool disabled() const { return m_disabled; }
        bool hidden() const { return m_hidden; }
        bool focused() const { return m_focused; }
        bool hovered() const { return m_hovered; }
        bool toBeDeleted() { return m_should_be_deleted; }

        Style style;
        int cursor = MOUSE_CURSOR_DEFAULT;

    protected:
        bool m_disabled = false;
        bool m_hidden = false;
        bool m_focused = false;
        bool m_hovered = false;
        bool m_should_be_deleted = false;
    };
} // namespace ui

#endif //INTERFACE_GUI_COMPONENTS_ABSTRACT_COMPONENT_H_
