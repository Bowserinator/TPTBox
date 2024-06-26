#include "Scene.h"
#include "../EventConsumer.h"
#include "../../util/vector_op.h"
#include "raylib.h"

#include <ranges>
#include <algorithm>
#include <vector>

using namespace ui;

Scene::~Scene() {
    for (auto &child : children) {
        delete child;
        child = nullptr;
    }
}

void Scene::removeChild(Component * component) {
    if (std::erase(children, component))
        delete component;
}

void Scene::removeChildDontDelete(Component * component) {
    std::erase(children, component);
}

void Scene::update() {
    float dt = GetFrameTime();
    bool mouseConsume = false;

    if (EventConsumer::ref()->getMouseDelta() != Vector2{0, 0})
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    // Preserve copy in case callback adds/removes children
    std::vector<Component *> childrenCopy(children.rbegin(), children.rend());
    for (auto child : childrenCopy) {
        // -- Input events
        Vector2 localPos = GetMousePosition() - pos;
        Vector2 childLocalPos = localPos - child->pos;
        bool containsCurrentMousePos = child->contains(childLocalPos);

        // -- Mouse movement ---
        if (EventConsumer::ref()->getMouseDelta() != Vector2{0, 0}) {
            Vector2 prevLocalPos = GetMousePosition() - EventConsumer::ref()->getMouseDelta() - pos;
            bool containsPrevMousePos = child->contains(prevLocalPos - child->pos);
            child->onMouseMoved(childLocalPos);

            if (containsCurrentMousePos && !containsPrevMousePos)
                child->onMouseEnter(childLocalPos);
            else if (!containsCurrentMousePos && containsPrevMousePos)
                child->onMouseLeave(childLocalPos);
        }

        // -- Mouse click --
        int keys[] = { MOUSE_BUTTON_LEFT, MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_BACK, MOUSE_BUTTON_FORWARD };
        #define MOUSE_CLICK_EVT(evt, childEvt) \
            if (EventConsumer::ref()->evt(key)) { \
                if (containsCurrentMousePos) { \
                    child->childEvt(childLocalPos, key); \
                    mouseConsume = true; \
                } \
            }

        for (auto key : keys) {
            MOUSE_CLICK_EVT(isMouseButtonPressed, onMouseClick);
            MOUSE_CLICK_EVT(isMouseButtonDown, onMouseDown);

            if (EventConsumer::ref()->isMouseButtonReleased(key)) {
                child->onMouseRelease(childLocalPos, key);
                mouseConsume = true;
            }
            if (EventConsumer::ref()->isMouseButtonReleased(key) && !containsCurrentMousePos)
                child->unfocus();
        }
        #undef MOUSE_CLICK_EVT

        // -- Mouse scroll --
        if (float scroll = EventConsumer::ref()->getMouseWheelMove()) {
            child->onMouseWheel(childLocalPos, scroll);
            if (containsCurrentMousePos)
                child->onMouseWheelInside(childLocalPos, scroll);
        }

        // -- Keys --
        child->updateKeys(
            EventConsumer::ref()->isKeyDown(KEY_LEFT_SHIFT),
            EventConsumer::ref()->isKeyDown(KEY_LEFT_CONTROL),
            EventConsumer::ref()->isKeyDown(KEY_LEFT_ALT));

        // -- Updates --
        child->tick(dt);
    }
    if (mouseConsume)
        EventConsumer::ref()->consumeMouse();

    ui::Scene::processChildrenDeletions(children);
}

void Scene::draw() {
    for (auto child : children)
        if (!child->hidden())
            child->draw(child->pos);
}
