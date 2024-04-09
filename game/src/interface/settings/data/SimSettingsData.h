#ifndef SIM_SETTING_DATA_H
#define SIM_SETTING_DATA_H

#include "../../../simulation/Simulation.h"

namespace settings {
    class Sim {
    public:
        Sim() {}
        Sim(Sim&other) = delete;
        void operator=(const Sim&) = delete;
        ~Sim() = default;

        bool enableHeat = true;
        bool enableAir = true;
        GravityMode gravityMode = GravityMode::VERTICAL;
        int threadCount = -1; // -1 is auto
    };
}

#endif