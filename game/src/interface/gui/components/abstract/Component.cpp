#include "Component.h"
#include "../../Scene.h"
#include "../Panel.h"

void ui::Component::tick(float dt) {}
void ui::Component::draw(const Vector2 &screenPos) { globalPos = screenPos; }

void ui::Component::onMouseMoved(Vector2 localPos) {}
void ui::Component::onMouseEnter(Vector2 localPos) {}
void ui::Component::onMouseLeave(Vector2 localPos) {}
void ui::Component::onMouseClick(Vector2 localPos, unsigned button) {}
void ui::Component::onMouseRelease(Vector2 localPos, unsigned button) {}

void ui::Component::onMouseDown(Vector2 localPos, unsigned button) {}
void ui::Component::onMouseWheel(Vector2 localPos, float d) {}
void ui::Component::onMouseWheelInside(Vector2 localPos, float d) {}
void ui::Component::updateKeys(bool shift, bool ctrl, bool alt) {}

void ui::Component::onFocus() {}
void ui::Component::onUnfocus() {}

void ui::Component::addToParent(ui::Component * child) {
    ui::Component * parent = this->parent;
    while (parent && parent->parent && dynamic_cast<ui::Panel*>(parent) == nullptr)
        parent = parent->parent;
    if (parent)
        ((ui::Panel*)parent)->addChild(child);
    else if (parentScene)
        parentScene->addChild(child);
}

void ui::Component::removeFromParent(ui::Component * child) {
    child->shouldBeDeleted = true;
}

void ui::Component::processDeletion() {}
