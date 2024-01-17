#ifndef EVENT_CONSUMER_H
#define EVENT_CONSUMER_H

#include "raylib.h"

/**
 * @brief Wrapper for a subset of Raylib's input events
 *        Checking for the event will stop propagation
 *        to future calls within the same frame
 */
class EventConsumer {
public:
    EventConsumer(EventConsumer&other) = delete;
    void operator=(const EventConsumer&) = delete;
    ~EventConsumer() = default;

    void reset();
    void consumeMouse();
    void consumeKeyboard();
    void consumeGamepad();

    // Input-related functions: keyboard
    bool isKeyPressed(int key);
    bool isKeyDown(int key);

    // Input-related functions: gamepads
    float getGamepadAxisMovement(const int gamepad, const int axis);
    bool isGamepadButtonPressed(const int gamepad, const int button);
    bool isGamepadButtonDown(const int gamepad, const int button);

    // Input-related functions: mouse
    bool isMouseButtonPressed(const int button);
    bool isMouseButtonDown(const int button);

    static EventConsumer * ref() {
        if (single == nullptr) [[unlikely]]
            single = new EventConsumer;
        return single;
    };

    static void destroy() {
        delete single;
        single = nullptr;
    }
private:
    bool keyboardEvtConsumed;
    bool gamepadEvtConsumed;
    bool mouseEvtConsumed;

    static EventConsumer * single;

    EventConsumer():
        keyboardEvtConsumed(false),
        gamepadEvtConsumed(false),
        mouseEvtConsumed(false) {}
};

#endif