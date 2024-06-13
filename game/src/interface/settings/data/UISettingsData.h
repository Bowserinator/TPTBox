#ifndef INTERFACE_SETTINGS_DATA_UISETTINGSDATA_H_
#define INTERFACE_SETTINGS_DATA_UISETTINGSDATA_H_

#include "AbstractSettingsData.h"

namespace settings {
    class UI : public AbstractSettingsData {
    public:
        UI() {}
        UI(const UI& other) = delete;
        void operator=(const UI&) = delete;
        ~UI() = default;

        enum class MovementMode { THREED = 0, FIRST_PERSON };
        enum class TemperatureUnit { C = 0, K = 1, F = 2 };

        virtual void loadFromJSON(const nlohmann::json &json) {
            if (json.contains("ui")) {
                brushOnFace = json["ui"].value("brushOnFace", brushOnFace);
                oppositeTool = json["ui"].value("oppositeTool", oppositeTool);
                frameIndependentCam = json["ui"].value("frameIndependentCam", frameIndependentCam);
                fastQuit = json["ui"].value("fastQuit", fastQuit);

                movementMode = static_cast<MovementMode>(
                    util::clamp(json["ui"].value("movementMode", (int)movementMode), 0, 1));
                temperatureUnit = static_cast<TemperatureUnit>(
                    util::clamp(json["ui"].value("temperatureUnit", (int)temperatureUnit), 0, 2));
                mouseSensitivity = util::clamp(json["ui"].value("mouseSensitivity", mouseSensitivity),
                    MIN_MOUSE_SENSITIVITY, MAX_MOUSE_SENSITIVITY);
            }
        }

        virtual void writeToJSON(nlohmann::json &json) const {
            json["ui"]["brushOnFace"] = brushOnFace;
            json["ui"]["oppositeTool"] = oppositeTool;
            json["ui"]["frameIndependentCam"] = frameIndependentCam;
            json["ui"]["fastQuit"] = fastQuit;
            json["ui"]["movementMode"] = (int)movementMode;
            json["ui"]["temperatureUnit"] = (int)temperatureUnit;
            json["ui"]["mouseSensitivity"] = mouseSensitivity;
        }

        bool brushOnFace = true;
        bool oppositeTool = true;
        bool frameIndependentCam = false;
        bool fastQuit = false;
        float mouseSensitivity = 1.0f;

        static constexpr float MIN_MOUSE_SENSITIVITY = 0.25f;
        static constexpr float MAX_MOUSE_SENSITIVITY = 2.0f;

        MovementMode movementMode = MovementMode::THREED;
        TemperatureUnit temperatureUnit = TemperatureUnit::C;
    };
} // namespace settings

#endif // INTERFACE_SETTINGS_DATA_UISETTINGSDATA_H_
