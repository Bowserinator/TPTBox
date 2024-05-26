#ifndef SIM_SETTING_DATA_H
#define SIM_SETTING_DATA_H

#include "AbstractSettingsData.h"
#include "../../../simulation/Simulation.h"

namespace settings {
    class Sim : public AbstractSettingsData {
    public:
        Sim() {}
        Sim(Sim&other) = delete;
        void operator=(const Sim&) = delete;
        ~Sim() = default;

        virtual void loadFromJSON(const nlohmann::json &json) {
            if (json.contains("sim")) {
                enableHeat = json["sim"].value("enableHeat", enableHeat);
                enableAir = json["sim"].value("enableAir", enableAir);
                threadCount = json["sim"].value("threadCount", threadCount);
                
                int tentativeGravityMode = json["sim"].value("gravityMode", (int)gravityMode);
                if (tentativeGravityMode >= 0 && tentativeGravityMode < (int)GravityMode::LAST)
                    gravityMode = (GravityMode)tentativeGravityMode;
            }
        }

        virtual void writeToJSON(nlohmann::json &json) const {
            json["sim"]["enableHeat"] = enableHeat;
            json["sim"]["enableAir"] = enableAir;
            json["sim"]["gravityMode"] = (int)gravityMode;
            json["sim"]["threadCount"] = threadCount;
        }

        bool enableHeat = true;
        bool enableAir = true;
        GravityMode gravityMode = GravityMode::VERTICAL;
        int threadCount = -1; // -1 is auto
    };
}

#endif