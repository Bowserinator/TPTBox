#include "Panel.h"
#include "../../EventConsumer.h"
#include "../../../util/vector_op.h"

#include <algorithm>

ui::Panel::Panel(const Vector2 &pos, const Vector2 &size): ui::Component(pos, size) {}

ui::Panel::~Panel() {
    for (auto child : children)
        delete child;
}

void ui::Panel::tick(float dt) {
    for (auto child : children)
        child->tick(dt);
}

void ui::Panel::draw(const Vector2 &screenPos) {
    if (hidden) return;
    for (auto child : children)
        child->draw(Vector2{ screenPos.x + child->pos.x, screenPos.y + child->pos.y });
}

void ui::Panel::addChild(Component * component) {
    component->setParent(this);
    children.push_back(component);
}

void ui::Panel::removeChild(Component * component) {
    auto itr = std::remove(children.begin(), children.end(), component);
    children.erase(itr, children.end());
    delete *itr;
}

ui::Component * ui::Panel::getChildIdx(std::size_t idx) {
    return children[idx];
}

std::size_t ui::Panel::getChildCount() const {
    return children.size();
}

void ui::Panel::updateGlobalControls() {
    Vector2 localpos  = GetMousePosition() - pos; // TODO: getGlobalPos somehow

    // TODO: do efficently
    // 1. get all elements that contain the point
    // iterate all indices
    // getAll mouse buttons that are down -> propogate
    // check scroll -> propogate to all and

    // OnMouseMove
    // OnMouseEnter (hover)
    // OnMouseLeave
    if (GetMouseDelta() != Vector2{0, 0}) {
        Vector2 prevlocal = GetMousePosition() - GetMouseDelta() - pos; // TODO global pos 

        for (auto child : children) {
            bool contains_now = child->contains(localpos - child->pos);
            bool contains_prev = child->contains(prevlocal - child->pos);

            if (contains_now)
                child->onMouseMoved(localpos.x - child->pos.x, localpos.y - child->pos.y);
            if (contains_now && !contains_prev)
                child->onMouseEnter(localpos.x - child->pos.x, localpos.y - child->pos.y);
            else if (!contains_now && contains_prev)
                child->onMouseLeave(localpos.x - child->pos.x, localpos.y - child->pos.y);
        }
    }

    // OnMouseDown
    // OnMouseUp
    // OnMouseClick

    // OnMouseWheel
    // OnMouseWheelInside
    // if (EventConsumer::ref()->isMouseButtonDown(MOUSE_BUTTON_LEFT));

    // keyup / down
}

// Propogate rest of the events
#define PROPOGATE_EVENT(evt) for (auto child : children) \
        if (child->contains(Vector2{ localx - child->pos.x, localy - child->pos.y })) \
            child->evt(localx - child->pos.x, localy - child->pos.y);
#define PROPOGATE_EVENT_VAR(evt, var) for (auto child : children) \
        if (child->contains(Vector2{ localx - child->pos.x, localy - child->pos.y })) \
            child->evt(localx - child->pos.x, localy - child->pos.y, var);

void ui::Panel::onMouseMoved(int localx, int localy) {
    PROPOGATE_EVENT(onMouseMoved)
}
void ui::Panel::onMouseEnter(int localx, int localy) {
    PROPOGATE_EVENT(onMouseEnter)
}
void ui::Panel::onMouseLeave(int localx, int localy) {
    PROPOGATE_EVENT(onMouseLeave)
}
void ui::Panel::onMouseDown(int localx, int localy, unsigned button) {
    PROPOGATE_EVENT_VAR(onMouseDown, button)
}
void ui::Panel::onMouseUp(int localx, int localy, unsigned button) {
    PROPOGATE_EVENT_VAR(onMouseUp, button)
}
void ui::Panel::onMouseClick(int localx, int localy, unsigned button) {
    PROPOGATE_EVENT_VAR(onMouseClick, button)
}
void ui::Panel::onMouseWheelInside(int localx, int localy, float d) {
    PROPOGATE_EVENT_VAR(onMouseWheelInside, d)
}

void ui::Panel::onMouseWheel(int localx, int localy, float d) {
    for (auto child : children)
        child->onMouseWheel(localx - child->pos.x, localy - child->pos.y, d);
}

void ui::Panel::onKeyDown(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) {
    for (auto child : children)
        child->onKeyDown(key, scan, repeat, shift, ctrl, alt);
}
void ui::Panel::onKeyUp(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) {
    for (auto child : children)
        child->onKeyUp(key, scan, repeat, shift, ctrl, alt);
}
