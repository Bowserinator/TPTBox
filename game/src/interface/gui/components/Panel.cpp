#include "Panel.h"
#include "../Scene.h"
#include "../../EventConsumer.h"
#include "../../../util/vector_op.h"

#include <ranges>
#include <algorithm>

ui::Panel::Panel(const Vector2 &pos, const Vector2 &size, const Style &style): ui::InteractiveComponent(pos, size, style) {}

ui::Panel::~Panel() {
    for (auto child : children)
        delete child;
}

void ui::Panel::tick(float dt) {
    for (auto child : children)
        child->tick(dt);
}

void ui::Panel::draw(const Vector2 &screenPos) {
    InteractiveComponent::draw(screenPos);
    for (auto child : children)
        if (!child->getHidden())
            child->draw(Vector2{ screenPos.x + child->pos.x, screenPos.y + child->pos.y });
}

void ui::Panel::addChild(Component * component) {
    component->setParent(this);
    component->setParentScene(parentScene);
    children.push_back(component);
}

void ui::Panel::setParentScene(Scene * scene) {
    parentScene = scene;
    for (auto child : children)
        child->setParentScene(scene);
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

// Propogate rest of the events
#define PROPOGATE_EVENT(evt) for (auto child : std::ranges::views::reverse(children)) \
        if (child->contains(localPos - child->pos)) { \
            child->evt(localPos - child->pos); \
            break; \
        }
#define PROPOGATE_EVENT_VAR(evt, var) for (auto child : std::ranges::views::reverse(children)) \
        if (child->contains(localPos - child->pos)) { \
            child->evt(localPos - child->pos, var); \
            break; \
        }

void ui::Panel::onMouseMoved(Vector2 localPos) {
    for (auto child : std::ranges::views::reverse(children))
        if (child->contains(localPos - child->pos))
            child->onMouseMoved(localPos - child->pos);

    Vector2 prevLocalPos = localPos - GetMouseDelta();
    for (auto child : std::ranges::views::reverse(children)) {
        if (child->contains(prevLocalPos - child->pos) && !child->contains(localPos - child->pos))
            child->onMouseLeave(localPos - child->pos);
        if (!child->contains(prevLocalPos - child->pos) && child->contains(localPos - child->pos))
            child->onMouseEnter(localPos - child->pos);
    }
}
void ui::Panel::onMouseEnter(Vector2 localPos) {
    PROPOGATE_EVENT(onMouseEnter)
}
void ui::Panel::onMouseLeave(Vector2 localPos) {
    Vector2 prevLocalPos = localPos - GetMouseDelta();
    for (auto child : std::ranges::views::reverse(children))
        if (child->contains(prevLocalPos - child->pos))
            child->onMouseLeave(localPos - child->pos);
}
void ui::Panel::onMouseDown(Vector2 localPos, unsigned button) {
    PROPOGATE_EVENT_VAR(onMouseDown, button)
}
void ui::Panel::onMouseUp(Vector2 localPos, unsigned button) {
    PROPOGATE_EVENT_VAR(onMouseUp, button)
}
void ui::Panel::onMouseClick(Vector2 localPos, unsigned button) {
    PROPOGATE_EVENT_VAR(onMouseClick, button)
}
void ui::Panel::onMouseWheelInside(Vector2 localPos, float d) {
    PROPOGATE_EVENT_VAR(onMouseWheelInside, d)
}

void ui::Panel::onMouseWheel(Vector2 localPos, float d) {
    for (auto child : std::ranges::views::reverse(children))
        child->onMouseWheel(localPos - child->pos, d);
}

void ui::Panel::updateKeys(bool shift, bool ctrl, bool alt) {
    for (auto child : std::ranges::views::reverse(children))
        child->updateKeys(shift, ctrl, alt);
}
