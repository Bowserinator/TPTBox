#ifndef SIMULATION_HEAT_H
#define SIMULATION_HEAT_H

#include "raylib.h"
#include "rlgl.h"
#include "SimulationDef.h"

class SimulationHeat {
public:
    ~SimulationHeat();

    void init();
    void dispatch();
    void wait_and_get();
private:
    unsigned int ssbo_in, ssbo_out;
    unsigned int golLogicShader;
    unsigned int golLogicProgram;
    float * out;
};

#endif