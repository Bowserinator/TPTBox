#include "Panel.h"
#include "../Scene.h"
#include "../../EventConsumer.h"
#include "../../../util/vector_op.h"

#include <ranges>
#include <algorithm>
#include <vector>

ui::Panel::Panel(const Vector2 &pos, const Vector2 &size, const Style &style):
    ui::InteractiveComponent(pos, size, style) {}

ui::Panel::~Panel() {
    clearChildren();
}

void ui::Panel::tick(float dt) {
    for (auto child : m_children)
        child->tick(dt);
}

void ui::Panel::draw(const Vector2 &screenPos) {
    if (style.backgroundColor.a)
        DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, style.backgroundColor);

    InteractiveComponent::draw(screenPos);
    for (auto child : m_children)
        if (!child->hidden())
            child->draw(Vector2{ screenPos.x + child->pos.x, screenPos.y + child->pos.y });
}

void ui::Panel::addChild(Component * component) {
    component->setParent(this);
    component->setParentScene(parentScene);
    m_children.push_back(component);
}

void ui::Panel::setParentScene(Scene * scene) {
    parentScene = scene;
    for (auto child : m_children)
        child->setParentScene(scene);
}

void ui::Panel::removeChild(Component * component) {
    if (std::erase(m_children, component))
        delete component;
}

void ui::Panel::clearChildren() {
    for (auto child : m_children)
        delete child;
    m_children.clear();
}

ui::Component * ui::Panel::getChildIdx(std::size_t idx) {
    return m_children[idx];
}

std::size_t ui::Panel::getChildCount() const {
    return m_children.size();
}

void ui::Panel::onUnfocus() {
    for (auto child : m_children)
        child->unfocus();
}

// Propogate rest of the events
#define PROPOGATE_EVENT(evt) std::vector<Component *> childrenCopy(m_children.rbegin(), m_children.rend()); \
    for (auto child : childrenCopy) \
        if (child->contains(localPos + m_event_delta - child->pos)) { \
            child->evt(localPos + m_event_delta - child->pos); \
            break; \
        }
#define PROPOGATE_EVENT_VAR(evt, var) std::vector<Component *> childrenCopy(m_children.rbegin(), m_children.rend()); \
    for (auto child : childrenCopy) \
        if (child->contains(localPos + m_event_delta - child->pos)) { \
            child->evt(localPos + m_event_delta - child->pos, var); \
            break; \
        }

void ui::Panel::onMouseMoved(Vector2 localPos) {
    std::vector<Component *> childrenCopy(m_children.rbegin(), m_children.rend());
    for (auto child : childrenCopy)
        child->onMouseMoved(localPos + m_event_delta - child->pos);

    Vector2 prevLocalPos = localPos - EventConsumer::ref()->getMouseDelta();
    for (auto child : childrenCopy) {
        bool containsNow = child->contains(localPos + m_event_delta - child->pos);
        bool containsPrev = child->contains(prevLocalPos + m_event_delta - child->pos);

        if (containsPrev && !containsNow)
            child->onMouseLeave(localPos + m_event_delta - child->pos);
        if (!containsPrev && containsNow)
            child->onMouseEnter(localPos + m_event_delta - child->pos);
    }
}
void ui::Panel::onMouseEnter(Vector2 localPos) {
    PROPOGATE_EVENT(onMouseEnter)
}
void ui::Panel::onMouseLeave(Vector2 localPos) {
    Vector2 prevLocalPos = localPos - EventConsumer::ref()->getMouseDelta();
    std::vector<Component *> childrenCopy(m_children.rbegin(), m_children.rend());
    for (auto child : childrenCopy)
        if (child->contains(prevLocalPos + m_event_delta - child->pos))
            child->onMouseLeave(localPos + m_event_delta - child->pos);
}
void ui::Panel::onMouseDown(Vector2 localPos, unsigned button) {
    PROPOGATE_EVENT_VAR(onMouseDown, button)
}
void ui::Panel::onMouseRelease(Vector2 localPos, unsigned button) {
    for (auto child : m_children)
        child->onMouseRelease(localPos + m_event_delta - child->pos, button);
}
void ui::Panel::onMouseClick(Vector2 localPos, unsigned button) {
    bool alreadyClicked = false;
    // Preserve copy, callback could modify
    std::vector<Component *> childrenCopy(m_children.rbegin(), m_children.rend());

    for (auto child : childrenCopy) {
        if (!alreadyClicked && child->contains(localPos + m_event_delta - child->pos)) {
            child->onMouseClick(localPos + m_event_delta - child->pos, button);
            alreadyClicked = true;
        } else {
            child->unfocus();
        }
    }
}
void ui::Panel::onMouseWheelInside(Vector2 localPos, float d) {
    PROPOGATE_EVENT_VAR(onMouseWheelInside, d)
}

void ui::Panel::onMouseWheel(Vector2 localPos, float d) {
    std::vector<Component *> childrenCopy(m_children.rbegin(), m_children.rend()); // Preserve copy, callback could modify
    for (auto child : childrenCopy)
        child->onMouseWheel(localPos + m_event_delta - child->pos, d);
}

void ui::Panel::updateKeys(bool shift, bool ctrl, bool alt) {
    std::vector<Component *> childrenCopy(m_children.rbegin(), m_children.rend()); // Preserve copy, callback could modify
    for (auto child : childrenCopy)
        child->updateKeys(shift, ctrl, alt);
}

void ui::Panel::processDeletion() {
    ui::Scene::processChildrenDeletions(m_children);
}
