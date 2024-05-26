#ifndef UI_SETTING_DATA_H
#define UI_SETTING_DATA_H

#include "AbstractSettingsData.h"

namespace settings {
    class UI : public AbstractSettingsData {
    public:
        UI() {}
        UI(UI&other) = delete;
        void operator=(const UI&) = delete;
        ~UI() = default;

        enum class MovementMode { THREED = 0, FIRST_PERSON };
        enum class TemperatureUnit { C = 0, K = 1, F = 2 };

        virtual void loadFromJSON(const nlohmann::json &json) {
            if (json.contains("ui")) {
                hideHud = json["ui"].value("hideHud", hideHud);
                oppositeTool = json["ui"].value("oppositeTool", oppositeTool);
                frameIndependentCam = json["ui"].value("frameIndependentCam", frameIndependentCam);
                fastQuit = json["ui"].value("fastQuit", fastQuit);

                movementMode = static_cast<MovementMode>(
                    util::clamp(json["ui"].value("movementMode", (int)movementMode), 0, 1));
                temperatureUnit = static_cast<TemperatureUnit>(
                    util::clamp(json["ui"].value("temperatureUnit", (int)temperatureUnit), 0, 2));
            }
        }

        virtual void writeToJSON(nlohmann::json &json) const {
            json["ui"]["hideHud"] = hideHud;
            json["ui"]["oppositeTool"] = oppositeTool;
            json["ui"]["frameIndependentCam"] = frameIndependentCam;
            json["ui"]["fastQuit"] = fastQuit;
            json["ui"]["movementMode"] = (int)movementMode;
            json["ui"]["temperatureUnit"] = (int)temperatureUnit;
        }

        bool hideHud = false;
        bool oppositeTool = true;
        bool frameIndependentCam = false;
        bool fastQuit = false;

        MovementMode movementMode = MovementMode::THREED;
        TemperatureUnit temperatureUnit = TemperatureUnit::C;
    };
}

#endif