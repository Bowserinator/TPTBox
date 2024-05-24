#ifndef UI_SETTING_DATA_H
#define UI_SETTING_DATA_H

namespace settings {
    class UI {
    public:
        UI() {}
        UI(UI&other) = delete;
        void operator=(const UI&) = delete;
        ~UI() = default;

        bool hideHud = false;
        bool oppositeTool = true;
        bool frameIndependentCam = false;
        bool fastQuit = false;

        enum class MovementMode { THREED = 0, FIRST_PERSON };
        enum class TemperatureUnit { C = 0, K = 1, F = 2 };

        MovementMode movementMode = MovementMode::THREED;
        TemperatureUnit temperatureUnit = TemperatureUnit::C;
    };
}

#endif