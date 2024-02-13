#include "Component.h"

void ui::Component::tick(float dt) {}
void ui::Component::draw(const Vector2 &screenPos) {}

void ui::Component::onMouseMoved(int localx, int localy) {}
void ui::Component::onMouseEnter(int localx, int localy) {}
void ui::Component::onMouseLeave(int localx, int localy) {}
void ui::Component::onMouseDown(int x, int y, unsigned button) {}
void ui::Component::onMouseUp(int x, int y, unsigned button) {}
void ui::Component::onMouseClick(int localx, int localy, unsigned button) {}
void ui::Component::onMouseWheel(int localx, int localy, float d) {}
void ui::Component::onMouseWheelInside(int localx, int localy, float d) {}
void ui::Component::onKeyDown(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) {}
void ui::Component::onKeyUp(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) {}

void ui::Component::onFocus() {}
void ui::Component::onDefocus() {}