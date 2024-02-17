#include "Scene.h"
#include "../EventConsumer.h"
#include "../../util/vector_op.h"
#include "raylib.h"

#include <ranges>
#include <algorithm>

using namespace ui;

Scene::~Scene() {
    for (auto child : children)
        delete child;
}

void Scene::removeChild(Component * component) {
    auto itr = std::remove(children.begin(), children.end(), component);
    children.erase(itr, children.end());
    delete *itr;
}

void Scene::update() {
    float dt = GetFrameTime();
    bool mouseConsume = false;

    if (GetMouseDelta() != Vector2{0, 0})
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    for (auto child : std::ranges::views::reverse(children)) {
        // -- Updates --
        child->tick(dt);

        // -- Input events
        Vector2 localPos = GetMousePosition() - pos;
        Vector2 childLocalPos = localPos - child->pos;
        bool containsCurrentMousePos = child->contains(childLocalPos);

        // -- Mouse movement ---
        if (GetMouseDelta() != Vector2{0, 0}) {
            Vector2 prevLocalPos = GetMousePosition() - GetMouseDelta() - pos;
            bool containsPrevMousePos = child->contains(prevLocalPos - child->pos);

            if (containsCurrentMousePos)
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
                if (!containsCurrentMousePos) { \
                    child->unfocus(); \
                } else { \
                    child->childEvt(childLocalPos, key); \
                    mouseConsume = true; \
                } \
            }

        for (auto key: keys) {
            MOUSE_CLICK_EVT(isMouseButtonPressed, onMouseClick);
            MOUSE_CLICK_EVT(isMouseButtonDown, onMouseDown);
            // MOUSE_CLICK_EVT(isMouseButtonUp, onMouseUp); // TODO
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
    }
    if (mouseConsume)
        EventConsumer::ref()->consumeMouse();
}

void Scene::draw() {
    for (auto child : children)
        if (!child->getHidden())
            child->draw(child->pos);
}
