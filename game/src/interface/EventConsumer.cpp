#include "EventConsumer.h"

EventConsumer * EventConsumer::single = nullptr;

void EventConsumer::reset() {
    keyboardEvtConsumed = false;
    gamepadEvtConsumed = false;
    mouseEvtConsumed = false;
}

void EventConsumer::consumeMouse() { mouseEvtConsumed = true; };
void EventConsumer::consumeKeyboard() { keyboardEvtConsumed = true; }
void EventConsumer::consumeGamepad() { gamepadEvtConsumed = true; }

// Input-related functions: keyboard
bool EventConsumer::isKeyPressed(int key) {
    if (keyboardEvtConsumed) return false;
    return IsKeyPressed(key);
}

bool EventConsumer::isKeyDown(int key) {
    if (keyboardEvtConsumed) return false;
    return IsKeyDown(key);
}

// Input-related functions: gamepads
float EventConsumer::getGamepadAxisMovement(const int gamepad, const int axis) {
    if (gamepadEvtConsumed) return false;
    return GetGamepadAxisMovement(gamepad, axis);
}

bool EventConsumer::isGamepadButtonPressed(const int gamepad, const int button) {
    if (gamepadEvtConsumed) return false;
    return IsGamepadButtonPressed(gamepad, button);
}

bool EventConsumer::isGamepadButtonDown(const int gamepad, const int button) {
    if (gamepadEvtConsumed) return false;
    return IsGamepadButtonDown(gamepad, button);
}

// Input-related functions: mouse
bool EventConsumer::isMouseButtonPressed(const int button) {
    if (mouseEvtConsumed) return false;
    return IsMouseButtonPressed(button);
}

bool EventConsumer::isMouseButtonDown(const int button) {
    if (mouseEvtConsumed) return false;
    return IsMouseButtonDown(button);
}

bool EventConsumer::isMouseButtonUp(const int button) {
    if (mouseEvtConsumed) return false;
    return IsMouseButtonUp(button);
}

float EventConsumer::getMouseWheelMove() {
    if (mouseEvtConsumed) return 0.0f;
    return GetMouseWheelMove();
}
