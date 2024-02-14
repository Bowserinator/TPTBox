#include "Component.h"

void ui::Component::tick(float dt) {}
void ui::Component::draw(const Vector2 &screenPos) { globalPos = screenPos; }

void ui::Component::onMouseMoved(Vector2 localPos) {}
void ui::Component::onMouseEnter(Vector2 localPos) {}
void ui::Component::onMouseLeave(Vector2 localPos) {}
void ui::Component::onMouseClick(Vector2 localPos, unsigned button) {}


void ui::Component::onMouseDown(Vector2 localPos, unsigned button) {}
void ui::Component::onMouseUp(Vector2 localPos, unsigned button) {}
void ui::Component::onMouseWheel(Vector2 localPos, float d) {}
void ui::Component::onMouseWheelInside(Vector2 localPos, float d) {}
void ui::Component::updateKeys(bool shift, bool ctrl, bool alt) {}

void ui::Component::onFocus() {}
void ui::Component::onUnfocus() {}