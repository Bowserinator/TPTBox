#ifndef GUI_COMPONENT_H
#define GUI_COMPONENT_H

#include "raylib.h"

namespace ui {
	class Panel;
    class Component {
    public:
        Component(const Vector2 &pos, const Vector2 &size): pos(pos), size(size) {};
        virtual ~Component() = default;

		virtual void tick(float dt);
		virtual void draw(const Vector2 &screenPos);

		virtual void onMouseMoved(int localx, int localy);
		virtual void onMouseEnter(int localx, int localy);
		virtual void onMouseLeave(int localx, int localy);
		virtual void onMouseDown(int x, int y, unsigned button);
		virtual void onMouseUp(int x, int y, unsigned button);
		virtual void onMouseClick(int localx, int localy, unsigned button);
		virtual void onMouseWheel(int localx, int localy, float d);
		virtual void onMouseWheelInside(int localx, int localy, float d);
		virtual void onKeyDown(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt);
		virtual void onKeyUp(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt);

		virtual void onFocus();
		virtual void onDefocus();
    
		void setParent(Panel * parent) { this->parent = parent; }
		
		// @param pos Local position inside the element
		inline bool contains(const Vector2 pos) const {
			return (pos.x >= 0 && pos.y >= 0 && pos.x <= size.x && pos.y <= size.y);
		}
		inline bool contains(Component * other) const {
			return ((pos.x < (other->pos.x + other->size.x) && (pos.x + size.x) > other->pos.x) &&
        			(pos.y < (other->pos.y + other->size.y) && (pos.y + size.y) > other->pos.y));
		}

        bool disabled = false;
        bool hidden = false;
        bool focused = false;
        Vector2 pos, size;
		Panel * parent;
    };
}

#endif